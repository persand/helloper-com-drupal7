MBP.scaleFix();
MBP.hideUrlBar();

(function ($) {

  Drupal.behaviors.helloper = {
    attach: function(context) {

      $.scrollDepth();

    }
  };

})(jQuery);