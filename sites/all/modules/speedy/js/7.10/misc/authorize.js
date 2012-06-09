/**
 * @file
 * Conditionally hide or show the appropriate settings and saved defaults
 * on the file transfer connection settings form used by authorize.php.
 */(function(a){Drupal.behaviors.authorizeFileTransferForm={attach:function(b){a("#edit-connection-settings-authorize-filetransfer-default").change(function(){a(".filetransfer").hide().filter(".filetransfer-"+a(this).val()).show()}),a(".filetransfer").hide().filter(".filetransfer-"+a("#edit-connection-settings-authorize-filetransfer-default").val()).show(),a(".connection-settings-update-filetransfer-default-wrapper").length>0&&a(".connection-settings-update-filetransfer-default-wrapper").css("float","none"),a("#edit-submit-connection").hide(),a("#edit-submit-process").show()}}})(jQuery);