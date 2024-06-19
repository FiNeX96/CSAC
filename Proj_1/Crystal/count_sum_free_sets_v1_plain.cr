MAX_N = 50

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

    # print the result
    (0..temp_n).each do |i|
        if i > 0
            state.@count[i] += state.@count[i-1]
        end
    end

    puts "# #{temp_n} #{state.@count[temp_n]} %.3e seconds" % (total_time.to_f)
end

(1..MAX_N).each do |i|
    state = State_t.new(i)
    run_solver(state, i)
end