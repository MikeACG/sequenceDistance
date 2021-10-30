// gets the minimum value from a vector and its position
// -1 values are treated as missing and never used as minimum
// unless all values of vector are missing
std::vector<int> minAndIdx(std::vector<int> v, int n) {

    int min = -1;
    int minIdx = -1;
    for (int i = 0; i < n; i++) {
        if (v[i] >= 0) { // valid distance (non-missing value)
            if (min < 0 || v[i] < min) { // replace undefined minimum or if defined minimum replace if new minimum
                min = v[i];
                minIdx = i;
            }
        }
    }

    std::vector<int> out = {min, minIdx};
    return out;
}