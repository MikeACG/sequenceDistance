// returns the block distance between two ranges, it is the minimum
// gap fom all possible gaps between the ranges, a gap of 1 position
// is defined as a 0 distance
int blockDist(int xstart, int xend, int ystart, int yend) {

    std::vector<int> diffs(4);

    diffs[0] = std::abs(xstart - ystart) - 1;
    diffs[1] = std::abs(xstart - yend) - 1;
    diffs[2] = std::abs(xend - ystart) - 1;
    diffs[3] = std::abs(xend - yend) - 1;

    int min = diffs[0];
    for (int i = 1; i < 4; i++) {
        if (diffs[i] < min) {
            min = diffs[i];
        }
    }

    return min;
}
