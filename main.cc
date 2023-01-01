#include <iostream>
#include <unistd.h>

class Board {
    class Buf {
        char *b;
        int width;
        int height;

      public:
        Buf(int h, int w) : width(w), height(h) {
            b = (char *)calloc(1, w * h);
            clear();
        }

        ~Buf() { free(b); }

        void clear(void) { memset(b, DEAD, width * height); }

        char get(int h, int w) { return b[h * width + w]; }
        void set(int h, int w, char c) { b[h * width + w] = c; }
        void copy(Buf &srcBuf) { memcpy(b, srcBuf.b, height * width); }
    };

    const int interval_us = 50 * 1000;

    enum state {
        LIVE = '@',
        DEAD = '.',
    };

    int width;
    int height;
    Buf buf;
    Buf nextBuf;

    void stepOne(void) {
        nextBuf.clear();

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                char c = buf.get(i, j);
                int n = adjLiveCount(i, j);
                if (c == LIVE) {
                    if (n == 2 || n == 3)
                        nextBuf.set(i, j, LIVE);
                } else {
                    if (n == 3)
                        nextBuf.set(i, j, LIVE);
                }
            }
        }

        buf.copy(nextBuf);
    }

    int adjLiveCount(int h, int w) {
        int upH = (h == 0) ? height - 1 : h - 1;
        int downH = (h == height - 1) ? 0 : h + 1;
        int leftW = (w == 0) ? width - 1 : w - 1;
        int rightW = (w == width - 1) ? 0 : w + 1;

        std::tuple<int, int> adjs[] = {
            {upH, leftW}, {upH, w},       {upH, rightW}, {h, leftW},
            {h, rightW},  {downH, leftW}, {downH, w},    {downH, rightW},
        };

        int count = 0;
        for (int i = 0; i < 8; i++) {
            auto [curH, curW] = adjs[i];
            if (buf.get(curH, curW) == LIVE)
                count++;
        }

        return count;
    }

    void print(void) {
        std::cout << "\x1b[d" << std::endl;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++)
                printf("%c", buf.get(i, j));
            printf("\n");
        }
    }

  public:
    Board(int h, int w) : width(w), height(h), buf(Buf(h, w)), nextBuf(h, w) {}

    ~Board(void) {}

    void plotGlider(int h, int w) {
        buf.set(h, w + 1, LIVE);
        buf.set(h + 1, w + 2, LIVE);
        buf.set(h + 2, w, LIVE);
        buf.set(h + 2, w + 1, LIVE);
        buf.set(h + 2, w + 2, LIVE);
    }

    void run(int nstep) {
        printf("\x1b[2J");
        for (int i = 0; i < nstep; i++) {
            print();

            int interval_us = 50 * 1000;
            usleep(interval_us);

            stepOne();
        }
    }
};

int main() {
    Board b = Board(40, 110);
    b.plotGlider(5, 10);
    b.run(1000);
}
