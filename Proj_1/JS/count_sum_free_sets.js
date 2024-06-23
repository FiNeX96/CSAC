// Get the command line argument for MAX_N, defaulting to 60 if not provided
const MAX_N = process.argv[2] ? parseInt(process.argv[2], 10) : 60;

let current_max, count, sum_count, numbers;

function main() {
    const start = Date.now();
    recurse(1, 0);
    const end = Date.now();
    const totalTimeSec = (end - start) / 1000;
    const totalTimeSecInt = Math.floor(totalTimeSec);

    for (let i=0; i<=current_max; i++) {
        if (i>0) {
            count[i] += count[i-1];
        }
    }
    console.log(`${current_max} ${count[current_max]} ${totalTimeSec.toFixed(6)} ${totalTimeSecInt}`);
}

function recurse(first_try, size) {
    for (;first_try <= current_max; first_try++) {
        if (sum_count[first_try] === 0) {
            count[first_try]++;
            numbers[size] = first_try;
            for (let i=0; i<=size; i++) {
                sum_count[first_try+numbers[i]]++;
            }
            recurse(first_try+1, size+1);
            for (let i=0; i<=size; i++) {
                sum_count[first_try+numbers[i]]--;
            }
        }
    }
}

for (let i = 1; i <= MAX_N; i++) {
    current_max = i;
    numbers = new Array(i);
    sum_count = new Array(2 * i + 1);
    count = new Array(i + 1);
    numbers.fill(0);
    sum_count.fill(0);
    count.fill(0);
    count[0] = 1;
    main();
}
