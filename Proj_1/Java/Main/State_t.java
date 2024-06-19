package Main;

public class State_t {
    public static int[] a;
    public static int[] sum_count;
    public static long[] count;

    public void create(int curr_n) {
        a = new int[curr_n];
        sum_count = new int[2 * curr_n + 1];
        count = new long[curr_n + 1];

    }
}
