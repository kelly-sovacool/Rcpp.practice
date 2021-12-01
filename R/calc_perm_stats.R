#' Get all performance metrics for test data
#' from mikropml
#' @param test_data Held out test data: dataframe of outcome and features.
#' @param trained_model Trained model from [caret::train()].
#' @param class_probs Whether to use class probabilities (TRUE for categorical outcomes, FALSE for numeric outcomes).
#' @inheritParams run_ml
#'
#' @return
#'
#' Dataframe of performance metrics.
#'
#' @export
#' @author Zena Lapp, \email{zenalapp@@umich.edu}
#'
#' @examples
#' \dontrun{
#' results <- run_ml(otu_small, "glmnet", kfold = 2, cv_times = 2)
#' calc_perf_metrics(results$test_data,
#'   results$trained_model,
#'   "dx",
#'   multiClassSummary,
#'   class_probs = TRUE
#' )
#' }
calc_perf_metrics_R <- function(test_data, trained_model, outcome_colname, perf_metric_function, class_probs) {
  pred_type <- "raw"
  if (class_probs) pred_type <- "prob"
  preds <- stats::predict(trained_model, test_data, type = pred_type)
  if (class_probs) {
    uniq_obs <- unique(c(test_data %>% dplyr::pull(outcome_colname), as.character(trained_model$pred$obs)))
    obs <- factor(test_data %>% dplyr::pull(outcome_colname), levels = uniq_obs)
    pred_class <- factor(names(preds)[apply(preds, 1, which.max)], levels = uniq_obs)
    perf_met <- perf_metric_function(data.frame(obs = obs, pred = pred_class, preds), lev = uniq_obs)
  } else {
    obs <- test_data %>% dplyr::pull(outcome_colname)
    perf_met <- perf_metric_function(data.frame(obs = obs, pred = preds))
  }
  return(perf_met)
}

#' Calculate performance for one permutation of one feature
#' from mikropML
calc_perm_perf_R <- function(feat_group, perm,
                           test_data, trained_model, outcome_colname,
                           perf_metric_function, perf_metric_name,
                           class_probs, progbar = NULL) {
    # permute grouped features together
    fs <- strsplit(feat_group, "\\|")[[1]]
    # get the new performance metric and performance metric differences
    n_rows <- nrow(test_data)
    # this strategy works for any number of features
    rows_shuffled <- sample(n_rows)
    permuted_test_data <- test_data
    permuted_test_data[, fs] <-
      permuted_test_data[rows_shuffled, fs]
    return(
      # TODO: It's wasteful to call caret::defaultSummary/MultiClassSummary,
      # which calculate several performance values, and then pull out only one
      # of them. Let's refactor this to just calculate the one we need.
      calc_perf_metrics(
          permuted_test_data,
          trained_model,
          outcome_colname,
          perf_metric_function,
          class_probs
        )[[perf_metric_name]]
    )
}

#' Calculate statistics for all permutations of a single feature
#' from mikropml
#' @param feat_group
#' @param perm_perfs
#' @param test_perf
#' @param progbar
#'
#' @return a named vector with `perf_metric` (value of the permuted performance
#'   metric), `perf_metric_diff`, `pvalue`, and `names`
#'
#' @export
#'
#' @examples
calc_perm_stats_R <- function(feat_group, perm_perfs, test_perf, progbar = NULL) {
  perm_perfs <- perm_perfs[names(perm_perfs) == feat_group]
  mean_perm_perf <- mean(perm_perfs)
  return(
    c(perf_metric = mean_perm_perf,
      perf_metric_diff = test_perf - mean_perm_perf,
      pvalue = calc_pvalue(perm_perfs, test_perf),
      names = feat_group
      )
    )
}

#' Calculate the p-value for a permutation test
#'
#' @param vctr vector of statistics
#' @param test_stat the test statistic
#'
#' @return the number of observations in `vctr` that are greater than
#'   `test_stat` divided by the number of observations in `vctr`
#'
#' @noRd
#' @author Kelly Sovacool \email{sovacool@@umich.edu}
calc_pvalue_R <- function(vctr, test_stat) {
  return(sum(vctr > test_stat) / length(vctr))
}
