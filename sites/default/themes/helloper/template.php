<?php
/**
 * Dispatch preprocesses to optional files.
 */
function helloper_preprocess(&$vars, $hook) {
  $filename = sprintf(
    '%s/inc/preprocess/preprocess-%s.inc',
    drupal_get_path('theme', 'helloper'),
    str_replace('_', '-', $hook)
  );

  if (is_file($filename)) {
    include($filename);
  }
}

/**
 * Dispatch processes to optional files.
 */
function helloper_process(&$vars, $hook) {
  $filename = sprintf(
    '%s/inc/process/process-%s.inc',
    drupal_get_path('theme', 'helloper'),
    str_replace('_', '-', $hook)
  );

  if (is_file($filename)) {
    include($filename);
  }
}

/**
 * Alter
 */

include(drupal_get_path('theme', 'helloper') . '/inc/alter.inc');

/**
 * Theme
 */

include(drupal_get_path('theme', 'helloper') . '/inc/theme.inc');

/**
 * Returns the star date
 *
 * Credits:
 * http://reusablecode.blogspot.com/2009/01/stardate.html
 */
function _helloper_stardate($timestamp) {
  $season = date("Y", $timestamp) - 1947;
  $episode = str_pad(round(1000 / 366 * (date("z", $timestamp) + 1)), 3, "0", STR_PAD_LEFT);
  $fractime = substr((date("g", $timestamp) * 60 + date("i", $timestamp)) / 144, 0, 1);
    
  return $season . $episode . "." . $fractime;
}

/**
 * Return a relative date
 *
 * Credits:
 * http://www.geekshangout.com/node/5
 */
function _helloper_relative_date($time) {
  $today = strtotime(date('M j, Y'));
  $reldays = ($time - $today)/86400;

  if ($reldays >= 0 && $reldays < 1) {
    return 'Today, ' . date("H:i", $time) . ' CET';
  } else if ($reldays >= 1 && $reldays < 2) {
    return 'Tomorrow';
  } else if ($reldays >= -1 && $reldays < 0) {
    return 'Yesterday, ' . date("H:i", $time) . ' CET';
  }

  if (abs($reldays) < 7) {
    if ($reldays > 0) {
      $reldays = floor($reldays);
      return 'In ' . $reldays . ' day' . ($reldays != 1 ? 's' : '');
    } else {
      $reldays = abs(floor($reldays));
      return $reldays . ' day' . ($reldays != 1 ? 's' : '') . ' ago';
    }
  }

  if (abs($reldays) < 182) {
    return date('l, j F',$time ? $time : time());
  } else {
    return date('l, j F, Y',$time ? $time : time());
  }
}
