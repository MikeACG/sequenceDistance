// determines if two ranges have at least one overlapping position
bool intersect(int xstart, int xend, int ystart, int yend) {

    if (ystart >= xstart && ystart <= xend) {
        return true;
    } else if (yend >= xstart && yend <= xend) {
        return true;
    }

    return false;
}
