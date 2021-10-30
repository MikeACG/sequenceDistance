// [[Rcpp::depends(RcppParallel)]]
#include <Rcpp.h>
#include <RcppParallel.h>
#include <intersect.h>
#include <blockDist.h>
#include <minAndIdx.h>
using namespace Rcpp;
using namespace RcppParallel;

struct DistComputer : public Worker {

    // inputs
    RMatrix<int> query, ref;
    int r;
    // ouputs
    RMatrix<int> out;

    // constructor
    DistComputer(IntegerMatrix query, IntegerMatrix ref, int r, IntegerMatrix out)
        : query(query), ref(ref), r(r), out(out) {}
    
    // logic 
    void operator()(std::size_t begin, std::size_t end) {
        
        bool overlap; // does the current pair of query-reference ranges overlap?
        std::vector<int> current(r); // keeps distances between a query range x and all reference ranges y
        std::vector<int> outRow(2);  // keeps the minimum distance between current query range x and any range y, also keeps index of y from within all ys

        for(std::size_t x = begin; x < end; x++){
            for(std::size_t y = 0; y < r; y++){
                if (query(x, 0) == ref(y, 0)) {
                    overlap = intersect(query(x, 1), query(x, 2), ref(y, 1), ref(y, 2));
                    current[y] = (overlap) ? 0 : 
                        blockDist(query(x, 1), query(x, 2), ref(y, 1), ref(y, 2));
                } else {
                    current[y] = -1; // NA value
                }
            }
            outRow = minAndIdx(current, r);
            out(x, 0) = outRow[0];
            out(x, 1) = outRow[1] + 1; // add 1 as indices are 1-based in R
        }
    }
};

// [[Rcpp::export]]
IntegerVector seqDists(IntegerMatrix query, IntegerMatrix ref) {
    int q = query.nrow();
    int r = ref.nrow();
    IntegerMatrix out(q, 2);
    DistComputer distComputer(query, ref, r, out);
    parallelFor(0, q, distComputer);
    return out;
}
