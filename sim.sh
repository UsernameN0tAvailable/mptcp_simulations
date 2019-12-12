#!/usr/bin/env bash



# MODE 1

ruby simulate.rb 0.0001 1 & ruby simulate.rb 0.00001 1
wait
ruby simulate.rb 0.000001 1 & ruby simulate.rb 0.0000001 1
wait
ruby simulate.rb 0.00000001 1 & ruby simulate.rb 0.000000001 1
wait


# MODE 2
ruby simulate.rb 0.5 2 & ruby simulate.rb 0.1 2
wait
ruby simulate.rb 0.05 2 & ruby simulate.rb 0.01 2
wait
ruby simulate.rb 0.001 2 & ruby simulate.rb 0.0001 2
wait
ruby simulate.rb 0.00001 2 & ruby simulate.rb 0.000001 2
wait


# MODE 3

ruby simulate.rb 0.01 3 & ruby simulate.rb 0.001 3
wait
ruby simulate.rb 0.0001 3 & ruby simulate.rb 0.00001 3
wait
ruby simulate.rb 0.000001 3 & ruby simulate.rb 0.0000001 3
wait
ruby simulate.rb 0.00000001 3 & ruby simulate.rb 0.000000001 3
wait


ruby simulate.rb 0.01 4 4 & ruby simulate.rb 0.01 4 4.5
wait
ruby simulate.rb 0.01 4 5 & ruby simulate.rb 0.01 4 5.5
wait
ruby simulate.rb 0.01 4 6 & ruby simulate.rb 0.01 4 6.5
wait
ruby simulate.rb 0.01 4 7 & ruby simulate.rb 0.01 4 8
wait