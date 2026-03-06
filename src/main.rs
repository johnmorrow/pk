use clap::Parser;
use std::fs::File;
use std::io::{self, BufRead, Write};
use std::process;

#[derive(Parser)]
#[command(name = "pk", version = "2.0.0", about = "A field extraction utility",
    after_help = "Optional-value flags (value must be directly attached, e.g. -Nfoo not -N foo):\n  \
                  -E[STRINGS]    Strings excluded from output (separated by :)\n  \
                  -N[STRING]     Change output text used for empty fields\n  \
                  -q[STRING]     Ignore delimiters within quotes")]
struct Cli {
    /// Backslash escapes delimiters
    #[arg(short = 'b', long = "backslash")]
    backslash: bool,

    /// Characters used as input delimiters
    #[arg(short = 'd', long = "delimiters")]
    delimiters: Option<String>,

    /// Allow empty fields
    #[arg(short = 'e', long = "empty")]
    empty: bool,

    /// Strings excluded from output (separated by :)
    #[arg(long = "excludes", hide = true)]
    excludes: Option<String>,

    /// Read input from file instead of stdin
    #[arg(short = 'f', long = "file")]
    file: Option<String>,

    /// Change output text used for empty fields
    #[arg(long = "null", hide = true)]
    null: Option<String>,

    /// Ignore delimiters within quotes
    #[arg(long = "quotes", hide = true)]
    quotes: Option<String>,

    /// Separator used in output text
    #[arg(short = 'S', long = "separator")]
    separator: Option<String>,

    /// Trim non-alphanumeric characters before printing
    #[arg(short = 'T', long = "trim")]
    trim: bool,

    /// Field specifications (indices, ranges, or literal strings)
    #[arg(required = true)]
    fields: Vec<String>,
}

// --- Configuration ---

struct Config {
    backslash: bool,
    delimiters: String,
    empty: bool,
    excludes: Vec<String>,
    null_string: Option<String>,
    quote_open: Option<char>,
    quote_close: Option<char>,
    separator: String,
    trim: bool,
    fields: Vec<FieldSpec>,
}

#[derive(Debug)]
enum FieldSpec {
    Number(usize),
    Range(usize, Option<usize>),
    Literal(String),
}

fn parse_field_spec(s: &str) -> FieldSpec {
    if let Some(dot_pos) = s.find("..") {
        let before = &s[..dot_pos];
        let after = &s[dot_pos + 2..];

        let start: Option<usize> = if before.is_empty() {
            Some(1)
        } else {
            parse_positive_number(before)
        };

        let end: Option<Option<usize>> = if after.is_empty() {
            Some(None)
        } else {
            parse_positive_number(after).map(Some)
        };

        if let (Some(s), Some(e)) = (start, end) {
            return FieldSpec::Range(s, e);
        }
    }

    if let Some(n) = parse_positive_number(s) {
        return FieldSpec::Number(n);
    }

    FieldSpec::Literal(s.to_string())
}

fn parse_positive_number(s: &str) -> Option<usize> {
    if s.is_empty() || !s.bytes().all(|b| b.is_ascii_digit()) {
        return None;
    }
    match s.parse::<usize>() {
        Ok(n) if n > 0 => Some(n),
        _ => None,
    }
}

fn convert_escaped_delimiters(input: &str) -> String {
    let mut output = String::with_capacity(input.len());
    let mut escaped = false;
    for ch in input.chars() {
        if !escaped && ch == '\\' {
            escaped = true;
        } else {
            if escaped {
                match ch {
                    't' => output.push('\t'),
                    'f' => output.push('\x0C'),
                    'n' => output.push('\n'),
                    'r' => output.push('\r'),
                    'v' => output.push('\x0B'),
                    _ => output.push(ch),
                }
            } else {
                output.push(ch);
            }
            escaped = false;
        }
    }
    output
}

fn parse_excludes(input: &str) -> Vec<String> {
    if input.is_empty() {
        return Vec::new();
    }
    let mut result = Vec::new();
    let mut current = String::new();
    let mut escaped = false;
    for ch in input.chars() {
        if escaped {
            current.push(ch);
            escaped = false;
        } else if ch == '\\' {
            escaped = true;
        } else if ch == ':' {
            result.push(std::mem::take(&mut current));
        } else {
            current.push(ch);
        }
    }
    result.push(current);
    result.retain(|s| !s.is_empty());
    result
}

fn build_config(cli: Cli) -> Config {
    let delimiters = match cli.delimiters {
        Some(d) => convert_escaped_delimiters(&d),
        None => "\t ".to_string(),
    };

    let excludes = match cli.excludes {
        Some(ref s) if s.is_empty() => Vec::new(),
        Some(ref s) => parse_excludes(s),
        None => match std::env::var("PK_EXCLUDES") {
            Ok(val) => parse_excludes(&val),
            Err(_) => Vec::new(),
        },
    };

    let null_string = match cli.null {
        Some(ref s) if s.is_empty() => None,
        Some(ref s) => Some(s.clone()),
        None => Some("NULL".to_string()),
    };

    let (quote_open, quote_close) = match cli.quotes {
        Some(ref s) => {
            let chars: Vec<char> = s.chars().collect();
            match chars.len() {
                0 => (Some('"'), Some('"')),
                1 => (Some(chars[0]), Some(chars[0])),
                _ => (Some(chars[0]), Some(chars[1])),
            }
        }
        None => (None, None),
    };

    let separator = cli.separator.unwrap_or_else(|| " ".to_string());
    let fields: Vec<FieldSpec> = cli.fields.iter().map(|s| parse_field_spec(s)).collect();

    Config {
        backslash: cli.backslash,
        delimiters,
        empty: cli.empty,
        excludes,
        null_string,
        quote_open,
        quote_close,
        separator,
        trim: cli.trim,
        fields,
    }
}

// --- Tokenizer ---

fn tokenize(line: &str, config: &Config) -> Vec<String> {
    let line = line.trim_end_matches(|c| c == '\n' || c == '\r');
    let chars: Vec<char> = line.chars().collect();
    let mut tokens: Vec<String> = Vec::new();
    let mut i = 0;
    let mut inside_token = false;
    let mut inside_quote = false;
    let mut previous_char_escape = false;
    let mut token_start: Option<usize> = None;

    while i < chars.len() {
        let ch = chars[i];
        let mut is_delimiter = config.delimiters.contains(ch);

        if is_delimiter && (previous_char_escape || inside_quote) {
            is_delimiter = false;
        }

        if !inside_token && !is_delimiter {
            token_start = Some(i);
        } else if is_delimiter && (inside_token || config.empty) {
            let raw = match token_start {
                Some(start) => &chars[start..i],
                None => &[],
            };
            tokens.push(process_token(raw, config));
            token_start = None;
        }

        previous_char_escape = ch == '\\' && !previous_char_escape;
        inside_token = !is_delimiter;

        if inside_token && !inside_quote {
            if let Some(qo) = config.quote_open {
                if ch == qo {
                    inside_quote = true;
                }
            }
        } else if inside_token && inside_quote {
            if let Some(qc) = config.quote_close {
                if ch == qc {
                    inside_quote = false;
                }
            }
        }

        i += 1;
    }

    if token_start.is_some() || config.empty {
        let raw = match token_start {
            Some(start) => &chars[start..],
            None => &[],
        };
        tokens.push(process_token(raw, config));
    }

    tokens
}

fn process_token(chars: &[char], config: &Config) -> String {
    let mut s: String = if config.backslash {
        remove_escape_chars(chars)
    } else {
        chars.iter().collect()
    };

    for exclude in &config.excludes {
        while let Some(pos) = s.find(exclude.as_str()) {
            s.replace_range(pos..pos + exclude.len(), "");
        }
    }

    if config.trim {
        trim_non_alnum(&mut s);
    }

    s
}

fn remove_escape_chars(chars: &[char]) -> String {
    let mut result = String::with_capacity(chars.len());
    let mut i = 0;
    while i < chars.len() {
        if chars[i] == '\\' {
            if i + 1 < chars.len() {
                if chars[i + 1] == '\\' {
                    result.push('\\');
                    i += 2;
                    continue;
                } else {
                    i += 1;
                    if i < chars.len() {
                        result.push(chars[i]);
                    }
                }
            }
            // Trailing backslash: skip
        } else {
            result.push(chars[i]);
        }
        i += 1;
    }
    result
}

fn trim_non_alnum(s: &mut String) {
    let start = s.find(|c: char| c.is_ascii_alphanumeric()).unwrap_or(s.len());
    s.drain(..start);
    let end = s
        .rfind(|c: char| c.is_ascii_alphanumeric())
        .map(|i| i + 1)
        .unwrap_or(0);
    s.truncate(end);
}

// --- Field Printer ---

fn print_fields(tokens: &[String], config: &Config, out: &mut impl Write) {
    let mut first = true;

    for field in &config.fields {
        match field {
            FieldSpec::Literal(s) => {
                output_string(&config.separator, s, &mut first, out);
            }
            FieldSpec::Number(n) => {
                output_field(tokens, n - 1, config, &mut first, out);
            }
            FieldSpec::Range(start, end) => {
                let idx_start = start - 1;
                let idx_end = match end {
                    Some(e) => e - 1,
                    None => {
                        if tokens.is_empty() {
                            0
                        } else {
                            tokens.len() - 1
                        }
                    }
                };
                let (lo, hi) = if idx_start <= idx_end {
                    (idx_start, idx_end)
                } else {
                    (idx_end, idx_start)
                };
                for i in lo..=hi {
                    output_field(tokens, i, config, &mut first, out);
                }
            }
        }
    }
    let _ = writeln!(out);
}

fn output_field(
    tokens: &[String],
    idx: usize,
    config: &Config,
    first: &mut bool,
    out: &mut impl Write,
) {
    if idx < tokens.len() && !tokens[idx].is_empty() {
        output_string(&config.separator, &tokens[idx], first, out);
    } else {
        match &config.null_string {
            Some(s) => output_string(&config.separator, s, first, out),
            None => {}
        }
    }
}

fn output_string(separator: &str, s: &str, first: &mut bool, out: &mut impl Write) {
    if *first {
        let _ = write!(out, "{}", s);
        *first = false;
    } else {
        let _ = write!(out, "{}{}", separator, s);
    }
}

// --- Argv preprocessing ---
// argp optional-arg options only consume the value when directly attached (-Nfoo).
// clap's num_args=0..=1 eagerly consumes the next separate argument.
// Fix by expanding -Xval to --long=val before clap sees it.

fn preprocess_argv() -> Vec<String> {
    let args: Vec<String> = std::env::args().collect();
    let mut result = Vec::with_capacity(args.len());
    result.push(args[0].clone());

    // (short_char, long_name, default_value_when_bare)
    let optional_arg_flags: &[(char, &str, &str)] = &[
        ('E', "excludes", ""),
        ('N', "null", ""),
        ('q', "quotes", "\""),
    ];

    let mut i = 1;
    while i < args.len() {
        let arg = &args[i];

        if arg.starts_with('-') && !arg.starts_with("--") && arg.len() >= 2 {
            let chars: Vec<char> = arg[1..].chars().collect();

            if let Some((_, long_name, default_val)) = optional_arg_flags
                .iter()
                .find(|(c, _, _)| chars[0] == *c)
            {
                if chars.len() > 1 {
                    // -Xvalue -> --long=value
                    let value: String = chars[1..].iter().collect();
                    result.push(format!("--{}={}", long_name, value));
                } else {
                    // Just -X alone -> --long=default
                    result.push(format!("--{}={}", long_name, default_val));
                }
                i += 1;
                continue;
            }
        }

        result.push(arg.clone());
        i += 1;
    }
    result
}

// --- Main ---

fn main() {
    let argv = preprocess_argv();
    let cli = Cli::parse_from(argv);
    let input_file = cli.file.clone();
    let config = build_config(cli);

    let stdin = io::stdin();
    let reader: Box<dyn BufRead> = match input_file {
        Some(ref path) => match File::open(path) {
            Ok(f) => Box::new(io::BufReader::new(f)),
            Err(e) => {
                eprintln!("pk: {}: {}", path, e);
                process::exit(1);
            }
        },
        None => Box::new(stdin.lock()),
    };

    let stdout = io::stdout();
    let mut out = io::BufWriter::new(stdout.lock());

    for line in reader.lines() {
        match line {
            Ok(line) => {
                let tokens = tokenize(&line, &config);
                print_fields(&tokens, &config, &mut out);
            }
            Err(e) => {
                eprintln!("pk: error reading input: {}", e);
                process::exit(1);
            }
        }
    }
}
