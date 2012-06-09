<?php

/**
 * @file
 * Drupad forms handling file
 *
 * This file makes managing FAPI forms easier on iOS side
 * Functions in this file are only intended as helpers or
 * replacements for Drupal core functions that don't do
 * exactly what we would except for us, which is most of the
 * time not implying the theme layer.
 */

function drupad_remove_form_groups_recursion(&$item) {
  $children = element_children($item);
  foreach ($children as $key) {
    if (isset($item[$key]['#groups'])) {
      unset($item[$key]['#groups']);
    }
    drupad_remove_form_groups_recursion($item[$key]);
  }
}

/**
 * Creates URL for managed_file which are refered by URI
 *
 * URI aren't really useful on iOS for now
 */
function drupad_convert_form_files_uri_to_url(&$item) {
  $children = element_children($item);
  foreach ($children as $key) {
    if (isset($item[$key]['#type']) && $item[$key]['#type'] == 'managed_file' && isset($item[$key]['#file']) && isset($item[$key]['#file']->uri)) {
      $item[$key]['#file']->drupad_url = file_create_url($item[$key]['#file']->uri);
    }
    drupad_convert_form_files_uri_to_url($item[$key]);
  }
}

/**
 * Applies the #pre_render callback recursively to a form and its children
 */
function drupad_form_apply_pre_render(&$item) {
  if (isset($item['#pre_render']) && is_array($item['#pre_render'])) {
    foreach ($item['#pre_render'] as $function) {
      if (function_exists($function)) {
        $item = $function($item);
      }
    }
  }
  $children_keys = element_children($item);
  foreach ($children_keys as $child_key) {
    drupad_form_apply_pre_render($item[$child_key]);
  }
}

/**
 * Converts form component's options to strict array
 *
 * Walk through each form component recursively for #options array.
 * Converts #options array to a strict array (not a associative array)
 * in order to keep the order revelant, as hashes don't have specific order.
 *
 * Also replace 0 value as "0" (string) so that it isn't interpreted as BOOL value
 * by Objective-C.
 *
 * @param &$form
 */
function drupad_convert_form_options_to_strict_array(&$item) {
  $children_keys = element_children($item);
  foreach ($children_keys as $child_key) {
    // Reached an optionable component (select or radios)
    if (isset($item[$child_key]['#options']) && is_array($item[$child_key]['#options'])) {
      $options = array();
      // Replaces #options with a strict array since
      // hashes don't keep order outside of PHP
      foreach ($item[$child_key]['#options'] as $key => $value) {
        if (is_object($value)) {
          $option = $value->option;
          reset($option); // Shouldn't be useful
          $options[] = array(key($option) => current($option));
        }
        else {
          // Dirty hack, but no choice
          if ($value === 0 && $key === 0) {
            $key = "0";
          }
          $options[] = array((string) $key => $value);
        }
      }
      // Shiny new options ordered
      $item[$child_key]['#options'] = $options;
    }
    // Continue deeper in the form item's structure
    drupad_convert_form_options_to_strict_array($item[$child_key]);
  }
}

