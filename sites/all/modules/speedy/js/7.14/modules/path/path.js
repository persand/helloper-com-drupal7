/**
 * @file
 * Attaches behaviors for the Path module.
 */(function(a){Drupal.behaviors.pathFieldsetSummaries={attach:function(b){a("fieldset.path-form",b).drupalSetSummary(function(b){var c=a(".form-item-path-alias input").val();return c?Drupal.t("Alias: @alias",{"@alias":c}):Drupal.t("No alias")})}}})(jQuery);