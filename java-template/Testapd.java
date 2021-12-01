import java.util.concurrent.CyclicBarrier;

public class Testapd {
    public static void main(String[] args) throws InterruptedException {
        if (args.length < 2) {
            System.err.println("Insufficient arguments");
            System.exit(-1);
        }

        int size = Integer.parseInt(args[0]);
        int numberOfThreads = Integer.parseInt(args[1]);

        Thread[] threads = new Thread[numberOfThreads];
        CyclicBarrier barrier = new CyclicBarrier(numberOfThreads);

        int[] array = new int[size];
        int[] results = new int[size];
        int[] ranks = new int[size];

        for (int i = 0; i < size; i++) {
            array[i] = size - i + 5;
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i] = new Task(numberOfThreads, size, i, array, ranks, results, barrier);
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i].start();
        }

        for (int i = 0; i < numberOfThreads; i++) {
            threads[i].join();
        }
    }
}
