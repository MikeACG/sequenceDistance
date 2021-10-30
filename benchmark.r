# required libraries
library(microbenchmark)
library(GenomicRanges) # v1.42.0
library(Rcpp)
library(RcppParallel)
Rcpp::sourceCpp("seqDists.cpp") # work on directory where this file is
# set number of threads
RcppParallel::setThreadOptions(numThreads = ceiling(RcppParallel::defaultNumThreads() * 0.75))
inputSize <- 500 # simulation size

# setup the query data for simulation
set.seed(123)
chromosomes <- sample(1:24, inputSize, replace = TRUE)
starts <- sample(seq(1, 3e+08, 300), inputSize)
ends <- starts + sample(10:300, inputSize, replace = TRUE)
query <- cbind(chromosomes, starts, ends)
storage.mode(query) <- "integer"

# setup the reference data for simulation
set.seed(456)
chromosomes <- sample(1:24, inputSize, replace = TRUE)
starts <- c(sample(starts, inputSize / 2), sample(seq(1, 3e+08, 300), inputSize / 2))
ends <- starts + sample(10:300, inputSize, replace = TRUE)
reference <- cbind(chromosomes, starts, ends)
storage.mode(reference) <- "integer"
grRef <- GenomicRanges::GRanges(
    seqnames = reference[, "chromosomes"], 
    ranges = IRanges::IRanges(
        start = reference[, "starts"], 
        end = reference[, "ends"]
    )
)

# this function will get the minimum distance and nearest neighbor
# in the reference of each genomic region in the query. The GenomicRanges
# package has to put one element of the query at a time in a GRanges object
# for the distance function to work
grf <- function(grRef, qry) {
    D <- lapply(1:nrow(qry), function(j) {
        grQuery <- GenomicRanges::GRanges(
            seqnames = qry[j, "chromosomes"], 
            ranges = IRanges::IRanges(
                start = qry[j, "starts"], 
                end = qry[j, "ends"]
            )
        )
        d <- GenomicRanges::distance(grRef, grQuery)
        if (all(is.na(d))) {
            min = NA
            idx = NA
        } else {
            min = min(d, na.rm = TRUE)
            idx = which.min(d)
        }
        c(dist = min, nn = idx)
    })
    return(do.call(rbind, D))
}
# this function will do the same but using a c++ routine
cppf <- function(ref, qry) {
    M <- seqDists(qry, ref)
    M[M[, 1] < 0, 1] <- NA  # NA distances are defined as -1 in c++ function
    M[M[, 2] < 1, 2] <- NA # NA neighbors are defined as 0 in c++ function
    colnames(M) <- c("dist", "nn")
    return(M)
}

# check that results are the same for both functions
a <- grf(grRef, query)
b <- cppf(reference, query)
identical(a, b)

# benchmarking
m <- microbenchmark::microbenchmark(
    grf(grRef, query), cppf(reference, query), 
    times = 10
)
m
