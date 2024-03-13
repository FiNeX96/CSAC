package Java;

public class Solver {
    static void recursive(int first_to_try, int set_size, State_t state) {

        Globals globals = new Globals();

        // try all ways to append one number to the current set
        for (;first_to_try <= globals.temp_n; first_to_try++) {
            if (state.sum_count[first_to_try] == 0) {
                // count it
                state.count[first_to_try]++;
                // update the set elements
                state.a[set_size] = first_to_try;
                // count new sums
                for(int i = 0;i <= set_size;i++) {
                    state.sum_count[state.a[i] + first_to_try]++;
                }
                // try one more
                recursive(first_to_try + 1, set_size + 1, state);
                // uncount sums
                for(int i = 0;i <= set_size;i++)
                state.sum_count[state.a[i] + first_to_try]--;
            }
        }
    }
    
    static void run_solver(State_t state) {
        Globals globals = new Globals();
            
        //  Count the empty set
        state.count[0] = Integer.toUnsignedLong(1);
        // count the rest
        long start_time = System.nanoTime();
        recursive(1, 0, state);
        long total_time = System.nanoTime() - start_time;
    
        //  Print the result
        for (int i = 0; i <= globals.temp_n; i++) {
            if (i>0) {
                state.count[i] += state.count[i-1];
            }
        }
    
        System.out.printf("# %d %d %6.3e seconds\n", globals.temp_n, state.count[globals.temp_n], (double)total_time / 1000000000);
    }
}
