require "option_parser"

# Default MAX_N value
DEFAULT_MAX_N = 50

# Command-line argument parsing
max_n = DEFAULT_MAX_N

OptionParser.parse do |parser|
  parser.banner = "Usage: crystal count_sum_free_sets_v1_plain.cr [options]"
  parser.on("-n MAX_N", "Set the maximum value for n (default: #{DEFAULT_MAX_N})") do |n|
    max_n = n.to_i
  end
end

struct State_t
  @a : Array(Int32)
  @sum_count : Array(Int32)
  @count : Array(Int32)

  def initialize(@curr_n : Int32)
    @a = Array(Int32).new(@curr_n) { 0 }
    @sum_count = Array(Int32).new(2 * @curr_n + 1) { 0 }
    @count = Array(Int32).new(@curr_n + 1) { 0 }
  end
end

def recursive(first_try : Int32, set_size : Int32, state : State_t)
  while first_try <= state.@a.size
    if state.@sum_count[first_try] == 0
      # count it
      state.@count[first_try] += 1
      # update the set elements
      state.@a[set_size] = first_try
      # count new sums
      (0..set_size).each do |i|
        state.@sum_count[first_try + state.@a[i]] += 1
      end
      # try one more
      recursive(first_try + 1, set_size + 1, state)
      # uncount sums
      (0..set_size).each do |i|
        state.@sum_count[first_try + state.@a[i]] -= 1
      end
    end
    # update vars
    first_try += 1
  end
end

  
def run_solver(state : State_t, temp_n : Int32)
  state.@count[0] = 1

  # count the rest
  start_time = Time.monotonic
  recursive(1, 0, state)
  end_time = Time.monotonic
  total_time = end_time - start_time
  total_time_int = total_time.total_seconds.to_i

  # print the result
  (0..temp_n).each do |i|
    if i > 0
      state.@count[i] += state.@count[i-1]
    end
  end

  puts "#{temp_n} #{state.@count[temp_n]} %.6f %d" % [total_time.to_f, total_time_int]
end

(1..max_n).each do |i|
  state = State_t.new(i)
  run_solver(state, i)
end