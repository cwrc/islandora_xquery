<?php

/**
 * @file
 * Functions for canceling the results of an xquery.
 */

/**
 * Queries the db for pending results and cancels them.
 *
 * @param int $batch_id
 *   Identifier for the batch that ran the xquery.
 */
function islandora_xquery_cancel_results($batch_id) {
  // Get the redirect url.
  $redirect = db_select('islandora_xquery_batches', 'batches')
    ->fields('batches', array('redirect'))
    ->condition('batch_id', $batch_id, '=')
    ->execute()
    ->fetchField();

  $result = db_select('islandora_xquery_diffs', 'diffs')
    ->fields('diffs', array('pid'))
    ->condition('batch_id', $batch_id)
    ->execute();

  module_load_include('inc', 'islandora_object_lock', 'includes/utilities');
  foreach ($result as $record) {
    islandora_object_lock_remove_object_lock($record->pid);
  }

  // Clear out the db.
  db_delete('islandora_xquery_diffs')
    ->condition('batch_id', $batch_id, '=')
    ->execute();

  db_delete('islandora_xquery_batches')
    ->condition('batch_id', $batch_id, '=')
    ->execute();

  // Redirect.
  $url = drupal_parse_url($redirect);
  drupal_goto($url['path'], $url);
}
