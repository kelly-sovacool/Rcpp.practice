
<!-- README.md is generated from README.Rmd. Please edit that file -->

# Rcpp.practice

Practice using the `Rcpp` package <!-- badges: start -->
<!-- badges: end -->

``` r
library(Rcpp)
```

See the C++ source code in [`src/`](src/).

### Advanced R chp. 25 sec. 1

Basic example

``` r
cppFunction('int add(int x, int y, int z) {
    int sum = x + y + z;
    return sum;
    }
')
add
#> function (x, y, z) 
#> .Call(<pointer: 0x107276900>, x, y, z)
add(1, 2, 3)
#> [1] 6
```

Loops aren’t as expensive in C++ as in R

``` r
sumR <- function(x) {
    total <- 0
    for (i in seq_along(x)) {
        total <- total + x[i]
    }
    total
}
cppFunction('double sumC(NumericVector x) {
    int n = x.size();
    double total = 0;
    for(int i = 0; i < n; ++i) {
        total += x[i];
    }
    return total;
}')
set.seed(19960207)
x <- runif(1e3)
sum_benchmark <- bench::mark(
    sum(x),
    sumC(x),
    sumR(x)
)
sum_benchmark[1:6]
#> # A tibble: 3 x 6
#>   expression      min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr> <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 sum(x)       1.82µs   2.83µs   210607.        0B     0   
#> 2 sumC(x)      4.27µs    8.9µs    67194.    2.49KB     6.72
#> 3 sumR(x)     39.27µs  42.62µs    20914.   19.23KB     0
```

Euclidean distance

``` r
pdistR <- function(x, ys) {
    sqrt((x - ys) ^ 2)
}
cppFunction('NumericVector pdistC(double x, NumericVector ys) {
    int n = ys.size();
    NumericVector out(n);
    for(int i = 0; i < n; ++i) {
        out[i] = sqrt(pow(ys[i] - x, 2.0));
    }
    return out;
}')
y <- runif(1e6)
bench::mark(
    pdistR(0.5, y),
    pdistC(0.5, y)
)[1:6]
#> Warning: Some expressions had a GC in every iteration; so filtering is disabled.
#> # A tibble: 2 x 6
#>   expression          min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>     <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 pdistR(0.5, y) 875.85ms    876ms      1.14    7.63MB     1.14
#> 2 pdistC(0.5, y)   9.61ms    310ms      3.23    7.63MB     1.61
```
