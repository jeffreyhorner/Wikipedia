#!/usr/bin/julia

t1 = t2 = lc = 0
d = Dict{ASCIIString,ASCIIString}()
f = open(ARGS[1])
while !eof(f)
  x = chomp(readline(f))
  start = time_ns()
  d[x] = "foo"
  t1 += (time_ns() - start)
  lc += 1
end
close(f)

miss = 0
f = open(ARGS[1])
while !eof(f)
  x = chomp(readline(f))
  start = time_ns()
  if haskey(d,x) && d[x] != "foo"
    miss += 1
  end
  t2 += (time_ns() - start)
end
close(f)

@printf(STDOUT,"julia\t%s\t%d\t%f\t%f\n" , ARGS[1],lc,t1/10^9,t2/10^9)
