# Handling broken or nested quoting is beyond the scope of the
# tool right now. This test is just to alert to changes in behaviour.
../../src/pk -q{} -d" ," 1
