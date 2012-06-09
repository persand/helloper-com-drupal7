Theme information:
------------------
Theme name: Hello Per
Author: Per Sandström
Author URL: http://www.helloper.com

Overview:
---------
Theme for www.helloper.com.

LICENSE:
--------
Dual licensed under GNU GPL v2 and MIT License.
LICENSE-GPL.txt
LICENSE-MIT.txt

CSS:
----
All files are combined into one compressed file. See inc/alter.inc.
The files are included in the following order:

css/vendor/normalize.css
See: https://github.com/necolas/normalize.css

css/globals.css
Global styles for the most common HTML elements and some helper classes.

css/vendor/foundation/grid.css
Grid from Zurb Foundation 3.0.

css/style.css
The main styling extending the globals.

css/ie.css
:(

css/print.css
Using @media print {}.

Javascript:
-----------
All files are combined into one compressed file. See inc/alter.inc.
Most of the files don't need an explanation.
Use minified files in production environment (use UglifyJS). 
Drupal will then aggregate all of them into one file.

js/vendor/jquery.scrolldepth.min.js
See: http://robflaherty.github.com/jquery-scrolldepth/

js/ie.js
Respond.js and Selectivizr 1.0.3b combined into a single file.
Only loaded for (lt IE 9) & (!IEMobile).
To be able to use both Respond.js and Selectivizr we must use Selectivizr 
1.0.3b.

TPL-files:
----------
Organized as good as possible in the tpl/ directory.