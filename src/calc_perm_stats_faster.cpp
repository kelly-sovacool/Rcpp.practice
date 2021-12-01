#include <Rcpp.h>
#include <algorithm>
using namespace Rcpp;

// [[Rcpp::export]]
double calc_pvalue_cpp(NumericVector perm_perfs, double test_stat) {
    double counts = std::count_if(perm_perfs.begin(), perm_perfs.end(),
                          [test_stat](double x) { return x > test_stat; });
    double pvalue = counts / perm_perfs.size();
    return pvalue;
}

/*** R
pval_cpp <- function() {
    lapply(seq.int(100), function() {
        perm_perfs <- rnorm(1000)
        test_stat <- sample(perm_perfs, 1)
        calc_pvalue_cpp(perm_perfs, test_stat)
    })
}
pval_R <- function() {
    lapply(seq.int(100), function() {
        perm_perfs <- rnorm(1000)
        test_stat <- sample(perm_perfs, 1)
        calc_pvalue_R(perm_perfs, test_stat)
    })
}
microbenchmark::microbenchmark(pval_cpp, pval_R, times = 100L)

perm_perfs <- rnorm(1000)
test_stat <- sample(perm_perfs, 1)
calc_pvalue_R(perm_perfs, test_stat)
calc_pvalue_cpp(perm_perfs, test_stat)
*/

// [[Rcpp::export]]
double calc_pvalue_cpp_for(NumericVector perm_perfs, double test_stat) {
    int counts = 0;
    NumericVector::iterator it;
    for(it = perm_perfs.begin(); it != perm_perfs.end(); ++it) {
        if(*it > test_stat) {
            counts++;
        }
    }
    double pvalue = (double)counts / perm_perfs.size();
    return pvalue;
}
/*** R

perm_perfs <- rnorm(1000)
test_stat <- sample(perm_perfs, 1)
calc_pvalue_R(perm_perfs, test_stat)
calc_pvalue_cpp_for(perm_perfs, test_stat)


pval_cpp_countif <- function() {
    lapply(seq.int(100), function() {
        perm_perfs <- rnorm(1000)
        test_stat <- sample(perm_perfs, 1)
        calc_pvalue_cpp(perm_perfs, test_stat)
    })
}
pval_cpp_for <- function() {
    lapply(seq.int(100), function() {
        perm_perfs <- rnorm(1000)
        test_stat <- sample(perm_perfs, 1)
        calc_pvalue_cpp_for(perm_perfs, test_stat)
    })
}
pval_R <- function() {
    lapply(seq.int(100), function() {
        perm_perfs <- rnorm(1000)
        test_stat <- sample(perm_perfs, 1)
        calc_pvalue_R(perm_perfs, test_stat)
    })
}
microbenchmark::microbenchmark(pval_R, pval_cpp, pval_cpp_for, times = 100000L)

*/
