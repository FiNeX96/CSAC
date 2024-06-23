use std::time::Duration;
use cpu_time::ProcessTime;

include!("max_n.rs");

// global computation state
struct State {
    a: [usize; MAX_N],                   // the set elements
    sum_count: [usize; 2 * MAX_N + 1], // the number of times a_i+a_j has been observed
    count: [usize; 1 + MAX_N],           // the number of subsets (indexed by its last element)
}

fn recurse(first_to_try: usize, set_size: usize, state: &mut State) {
    // try all ways to append one number to the current set
    for i in first_to_try..=MAX_N {
        if state.sum_count[i] == 0 {
            // count it
            state.count[i] += 1;
            // update the set elements
            state.a[set_size] = i;
            // count new sums
            for j in 0..=set_size {
                state.sum_count[state.a[j] + i] += 1;
            }
            // try one more
            recurse(i + 1, set_size + 1, state);
            // uncount sums
            for j in 0..=set_size {
                state.sum_count[state.a[j] + i] -= 1;
            }
        }
    }
}

fn main() {
    // count the empty set
    let mut state = State {
        a: [0; MAX_N],
        sum_count: [0; 2 * MAX_N + 1],
        count: [0; 1 + MAX_N],
    };
    state.count[0] = 1;
    // count the rest
    let start_time = std::time::Instant::now();
    let start = ProcessTime::now();
    recurse(1, 0, &mut state);
    let cpu_time: Duration = start.elapsed();
    let elapsed_time = start_time.elapsed();
    // report
    for i in 0..=MAX_N {
        if i > 0 {
            state.count[i] += state.count[i - 1];
        }
        println!("{} {}", i, state.count[i]);
    }
    println!(
        "# {} {} {:.6} {}",
        MAX_N,
        state.count[MAX_N],
        cpu_time.as_secs_f64(),
        elapsed_time.as_secs()
    );
}
