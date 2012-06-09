/**
 * jQuery Once Plugin v1.2
 * http://plugins.jquery.com/project/once
 *
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 */(function(a){var b={},c=0;a.fn.once=function(d,e){typeof d!="string"&&(d in b||(b[d]=++c),e||(e=d),d="jquery-once-"+b[d]);var f=d+"-processed",g=this.not("."+f).addClass(f);return a.isFunction(e)?g.each(e):g},a.fn.removeOnce=function(b,c){var d=b+"-processed",e=this.filter("."+d).removeClass(d);return a.isFunction(c)?e.each(c):e}})(jQuery);