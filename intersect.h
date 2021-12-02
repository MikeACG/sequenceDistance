// determines if two ranges have at least one overlapping position
bool intersect(int xstart, int xend, int ystart, int yend) {

    if (xstart >= ystart && xstart <= yend) {
        return true;
    } else if (xend >= ystart && xend <= yend) {
        return true;
    }

    return false;
}
