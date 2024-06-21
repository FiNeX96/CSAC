package Main;

public class Main {
    public static void main(String[] args) {

        Globals globals = new Globals();

        if (args.length > 0) {
            try {
                int maxN = Integer.parseInt(args[0]);
                globals.max_n = maxN;
            } catch (NumberFormatException e) {
                System.err.println("Argument must be an intenger");
                System.exit(-1);
            }
        }

        for (int i = 0; i <= globals.max_n; i++) {
            State_t state = new State_t();
            state.create(i);
            globals.temp_n = i;
            Solver.run_solver(state);
        }
    }
}
