import java.util.Arrays;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class Task extends Thread {
    private final int numberOfThreads;
    private final int size;
    private final int id;
    private final int[] array;
    private final int[] rank;
    private final int[] result;
    private final CyclicBarrier barrier;

    public Task(int numberOfThreads, int size, int id, int[] array, int[] rank, int[] result, CyclicBarrier barrier) {
        this.numberOfThreads = numberOfThreads;
        this.size = size;
        this.id = id;
        this.array = array;
        this.rank = rank;
        this.result = result;
        this.barrier = barrier;
    }

    @Override
    public void run() {
        int start = id * (int) Math.ceil((double) size / (double) numberOfThreads);
        int end = Math.min((id + 1) * (int) Math.ceil((double) size / (double) numberOfThreads), size);

        for (int i = start; i < end; i++) {
            for (int j = 0; j < size; j++) {
                if (array[i] > array[j]) {
                    rank[i]++;
                }
            }
        }

        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        for (int i = start; i < end; i++) {
            result[rank[i]] = array[i];
        }

        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        if (id == 0) {
            boolean isCorrectlySorted = true;

            for (int i = 0; i < size - 1; i++) {
                if (result[i] > result[i + 1]) {
                    isCorrectlySorted = false;
                    break;
                }
            }

            if (isCorrectlySorted) {
                System.out.println("Sorted correctly");
            } else {
                System.out.println("Sorted incorrectly");
            }

            System.out.println(Arrays.toString(result));
        }
    }
}
