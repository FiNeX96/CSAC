package Java;

public class Main {
    public static void main(String[] args) {

        Globals globals = new Globals();

        for (int i = 0; i <= globals.max_n; i++) {
            State_t state = new State_t();
            state.create(i);
            globals.temp_n = i;
            Solver.run_solver(state);
        }

    }
}