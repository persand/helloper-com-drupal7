<?php

/**
 * @file
 * Hooks provided by the Field formatter settings module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter the form elements for a formatter's settings.
 *
 * @param $settings_form
 *   The settings form array from the field module's implementation of
 *   hook_field_formatter_settings().
 * @param $context
 *   An array of additional context for the settings form.
 *   - field: The field structure being configured.
 *   - instance: The instance structure being configured.
 *   - view_mode: The view mode being configured.
 *   - form: The (entire) configuration form array, which will usually have no
 *     use here.
 *   - form_state: The form state of the (entire) configuration form.
 *
 * @see hook_field_formatter_settings()
 */
function hook_field_formatter_settings_form_alter(array &$settings_form, array $context) {

}

/**
 * Alter the short summary for the current formatter settings of an instance.
 *
 * @param $summary
 *   A string containing a short summary of the formatter settings.
 * @param $context
 *   An array with additional context for the summary:
 *   - field: The field structure.
 *   - instance: The instance structure.
 *   - view_mode: The view mode for which a settings summary is requested.
 *
 * @see hook_field_formatter_settings_summary()
 */
function hook_field_formatter_settings_summary_alter(&$sumary, array $context) {

}

/**
 * @} End of "addtogroup hooks".
 */
