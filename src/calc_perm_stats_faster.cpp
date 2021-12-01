#include <Rcpp.h>
#include <algorithm>
#include <random>
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
double calc_pvalue_cpp_vctr(NumericVector perm_perfs, double test_stat) {
    NumericVector above_stat = perm_perfs[perm_perfs > test_stat];
    double pvalue = (double)above_stat.size() / perm_perfs.size();
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
pval_cpp_vctr <- function() {
    lapply(seq.int(100), function() {
        perm_perfs <- rnorm(1000)
        test_stat <- sample(perm_perfs, 1)
        calc_pvalue_cpp_vctr(perm_perfs, test_stat)
    })
}
pval_R <- function() {
    lapply(seq.int(100), function() {
        perm_perfs <- rnorm(1000)
        test_stat <- sample(perm_perfs, 1)
        calc_pvalue_R(perm_perfs, test_stat)
    })
}
microbenchmark::microbenchmark(pval_R, pval_cpp, pval_cpp_for, pval_cpp_vctr,
                               times = 100L)

*/

Rcpp::IntegerVector sample_int(int n, int min, int max) {
    // from https://stackoverflow.com/questions/41040040/generating-sample-of-integers-in-rcpp
    // and https://en.cppreference.com/w/cpp/algorithm/random_shuffle
    Rcpp::IntegerVector pool = Rcpp::seq(min, max);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(pool.begin(), pool.end(), g);
    return pool;
}

// [[Rcpp::export]]
DataFrame permute_cpp(DataFrame test_data, CharacterVector feats_group) {

    int num_rows = test_data.nrows();
    IntegerVector idx = sample_int(num_rows, 0, num_rows-1);

    CharacterVector::iterator feat;
    for (feat = feats_group.begin(); feat != feats_group.end(); feat++) {
        String curr_feat = *feat;
        NumericVector x = test_data[curr_feat];
        // reorder the rows in this column
        test_data[curr_feat] = x[idx];
    }
    std::cout << idx << std::endl;

    return test_data;
}


/*** R
test_df <- data.frame(a = 1:5, b = 6:10, c = 11:15, d = 16:20)
group_feats <- c('a', 'c')
permute_R <- function(test_data, group_feats) {
    rows_shuffled <- sample(nrow(test_data))
    test_data[, group_feats] <-
        test_data[rows_shuffled, group_feats]
    return(test_data)
}
microbenchmark::microbenchmark(permute_R(test_df, group_feats),
                               permute_cpp(test_df, group_feats))
run_permute_R <- function() {
    test_df <- data.frame(replicate(1000,sample(0:1,1000,rep=TRUE)))
    group_feats <- c('X1', 'X2', 'X3', 'X4')
    permute_R(test_df, group_feats)
}
run_permute_cpp <- function() {
    test_df <- data.frame(replicate(1000,sample(0:1,1000,rep=TRUE)))
    group_feats <- c('X1', 'X2', 'X3', 'X4')
    permute_cpp(test_df, group_feats)
}
microbenchmark::microbenchmark(run_permute_R(),
                               run_permute_cpp(), times = 10L)
*/
