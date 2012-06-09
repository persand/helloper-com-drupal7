/**
 * Internet Explorer specific JS (lt IE 9)
 *
 * Selectivzr 1.0.3
 * Respond.js 1.1.0
 */

/*
selectivizr v1.0.3b - (c) Keith Clark, freely distributable under the terms 
of the MIT license.

selectivizr.com
*/
/* 
  
Notes about this source
-----------------------

 * The #DEBUG_START and #DEBUG_END comments are used to mark blocks of code
   that will be removed prior to building a final release version (using a
   pre-compression script)
  
  
References:
-----------
 
 * CSS Syntax          : http://www.w3.org/TR/2003/WD-css3-syntax-20030813/#style
 * Selectors           : http://www.w3.org/TR/css3-selectors/#selectors
 * IE Compatability    : http://msdn.microsoft.com/en-us/library/cc351024(VS.85).aspx
 * W3C Selector Tests  : http://www.w3.org/Style/CSS/Test/CSS3/Selectors/current/html/tests/
 
*/

(function(win) {

  // If browser isn't IE, then stop execution! This handles the script 
  // being loaded by non IE browsers because the developer didn't use 
  // conditional comments.
  if (/*@cc_on!@*/true) return;

  // =========================== Init Objects ============================

  var doc = document;
  var root = doc.documentElement;
  var xhr = getXHRObject();
  var ieVersion = /MSIE (\d+)/.exec(navigator.userAgent)[1];
  
  // If were not in standards mode, IE is too old / new or we can't create
  // an XMLHttpRequest object then we should get out now.
  if (doc.compatMode != 'CSS1Compat' || ieVersion<6 || ieVersion>8 || !xhr) {
    return;
  }
  
  
  // ========================= Common Objects ============================

  // Compatiable selector engines in order of CSS3 support. Note: '*' is
  // a placholder for the object key name. (basically, crude compression)
  var selectorEngines = {
    "NW"                : "*.Dom.select",
    "MooTools"              : "$$",
    "DOMAssistant"            : "*.$", 
    "Prototype"             : "$$",
    "YAHOO"               : "*.util.Selector.query",
    "Sizzle"              : "*", 
    "jQuery"              : "*",
    "dojo"                : "*.query"
  };

  var selectorMethod;
  var enabledWatchers           = [];     // array of :enabled/:disabled elements to poll
  var domPatches              = [];
  var ie6PatchID              = 0;      // used to solve ie6's multiple class bug
  var patchIE6MultipleClasses       = true;   // if true adds class bloat to ie6
  var namespace               = "slvzr";

  // Stylesheet parsing regexp's
  var RE_COMMENT              = /(\/\*[^*]*\*+([^\/][^*]*\*+)*\/)\s*?/g;
  var RE_IMPORT             = /@import\s*(?:(?:(?:url\(\s*(['"]?)(.*)\1)\s*\))|(?:(['"])(.*)\3))\s*([^;]*);/g;
  var RE_ASSET_URL            = /\burl\(\s*(["']?)(?!data:)([^"')]+)\1\s*\)/g;
  var RE_PSEUDO_STRUCTURAL        = /^:(empty|(first|last|only|nth(-last)?)-(child|of-type))$/;
  var RE_PSEUDO_ELEMENTS          = /:(:first-(?:line|letter))/g;
  var RE_SELECTOR_GROUP         = /((?:^|(?:\s*})+)(?:\s*@media[^{]+{)?)\s*([^\{]*?[\[:][^{]+)/g;
  var RE_SELECTOR_PARSE         = /([ +~>])|(:[a-z-]+(?:\(.*?\)+)?)|(\[.*?\])/g; 
  var RE_LIBRARY_INCOMPATIBLE_PSEUDOS   = /(:not\()?:(hover|enabled|disabled|focus|checked|target|active|visited|first-line|first-letter)\)?/g;
  var RE_PATCH_CLASS_NAME_REPLACE     = /[^\w-]/g;
  
  // HTML UI element regexp's
  var RE_INPUT_ELEMENTS         = /^(INPUT|SELECT|TEXTAREA|BUTTON)$/;
  var RE_INPUT_CHECKABLE_TYPES      = /^(checkbox|radio)$/;

  // Broken attribute selector implementations (IE7/8 native [^=""], [$=""] and [*=""])
  var BROKEN_ATTR_IMPLEMENTATIONS     = ieVersion>6 ? /[\$\^*]=(['"])\1/ : null;

  // Whitespace normalization regexp's
  var RE_TIDY_TRAILING_WHITESPACE     = /([(\[+~])\s+/g;
  var RE_TIDY_LEADING_WHITESPACE      = /\s+([)\]+~])/g;
  var RE_TIDY_CONSECUTIVE_WHITESPACE    = /\s+/g;
  var RE_TIDY_TRIM_WHITESPACE       = /^\s*((?:[\S\s]*\S)?)\s*$/;
  
  // String constants
  var EMPTY_STRING            = "";
  var SPACE_STRING            = " ";
  var PLACEHOLDER_STRING          = "$1";

  // =========================== Patching ================================

  // --[ patchStyleSheet() ]----------------------------------------------
  // Scans the passed cssText for selectors that require emulation and
  // creates one or more patches for each matched selector.
  function patchStyleSheet( cssText ) {
    return cssText.replace(RE_PSEUDO_ELEMENTS, PLACEHOLDER_STRING).
      replace(RE_SELECTOR_GROUP, function(m, prefix, selectorText) {  
          var selectorGroups = selectorText.split(",");
          for (var c = 0, cs = selectorGroups.length; c < cs; c++) {
            var selector = normalizeSelectorWhitespace(selectorGroups[c]) + SPACE_STRING;
            var patches = [];
            selectorGroups[c] = selector.replace(RE_SELECTOR_PARSE, 
              function(match, combinator, pseudo, attribute, index) {
                if (combinator) {
                  if (patches.length>0) {
                    domPatches.push( { selector: selector.substring(0, index), patches: patches } )
                    patches = [];
                  }
                  return combinator;
                }   
                else {
                  var patch = (pseudo) ? patchPseudoClass( pseudo ) : patchAttribute( attribute );
                  if (patch) {
                    patches.push(patch);
                    return "." + patch.className;
                  }
                  return match;
                }
              }
            );
          }
          return prefix + selectorGroups.join(",");
        });
  };

  // --[ patchAttribute() ]-----------------------------------------------
  // returns a patch for an attribute selector.
  function patchAttribute( attr ) {
    return (!BROKEN_ATTR_IMPLEMENTATIONS || BROKEN_ATTR_IMPLEMENTATIONS.test(attr)) ? 
      { className: createClassName(attr), applyClass: true } : null;
  };

  // --[ patchPseudoClass() ]---------------------------------------------
  // returns a patch for a pseudo-class
  function patchPseudoClass( pseudo ) {

    var applyClass = true;
    var className = createClassName(pseudo.slice(1));
    var isNegated = pseudo.substring(0, 5) == ":not(";
    var activateEventName;
    var deactivateEventName;

    // if negated, remove :not() 
    if (isNegated) {
      pseudo = pseudo.slice(5, -1);
    }
    
    // bracket contents are irrelevant - remove them
    var bracketIndex = pseudo.indexOf("(")
    if (bracketIndex > -1) {
      pseudo = pseudo.substring(0, bracketIndex);
    }   
    
    // check we're still dealing with a pseudo-class
    if (pseudo.charAt(0) == ":") {
      switch (pseudo.slice(1)) {

        case "root":
          applyClass = function(e) {
            return isNegated ? e != root : e == root;
          }
          break;

        case "target":
          // :target is only supported in IE8
          if (ieVersion == 8) {
            applyClass = function(e) {
              var handler = function() { 
                var hash = location.hash;
                var hashID = hash.slice(1);
                return isNegated ? (hash == EMPTY_STRING || e.id != hashID) : (hash != EMPTY_STRING && e.id == hashID);
              };
              addEvent( win, "hashchange", function() {
                toggleElementClass(e, className, handler());
              })
              return handler();
            }
            break;
          }
          return false;
        
        case "checked":
          applyClass = function(e) { 
            if (RE_INPUT_CHECKABLE_TYPES.test(e.type)) {
              addEvent( e, "propertychange", function() {
                if (event.propertyName == "checked") {
                  toggleElementClass( e, className, e.checked !== isNegated );
                }               
              })
            }
            return e.checked !== isNegated;
          }
          break;
          
        case "disabled":
          isNegated = !isNegated;

        case "enabled":
          applyClass = function(e) { 
            if (RE_INPUT_ELEMENTS.test(e.tagName)) {
              addEvent( e, "propertychange", function() {
                if (event.propertyName == "$disabled") {
                  toggleElementClass( e, className, e.$disabled === isNegated );
                } 
              });
              enabledWatchers.push(e);
              e.$disabled = e.disabled;
              return e.disabled === isNegated;
            }
            return pseudo == ":enabled" ? isNegated : !isNegated;
          }
          break;
          
        case "focus":
          activateEventName = "focus";
          deactivateEventName = "blur";
                
        case "hover":
          if (!activateEventName) {
            activateEventName = "mouseenter";
            deactivateEventName = "mouseleave";
          }
          applyClass = function(e) {
            addEvent( e, isNegated ? deactivateEventName : activateEventName, function() {
              toggleElementClass( e, className, true );
            })
            addEvent( e, isNegated ? activateEventName : deactivateEventName, function() {
              toggleElementClass( e, className, false );
            })
            return isNegated;
          }
          break;
          
        // everything else
        default:
          // If we don't support this pseudo-class don't create 
          // a patch for it
          if (!RE_PSEUDO_STRUCTURAL.test(pseudo)) {
            return false;
          }
          break;
      }
    }
    return { className: className, applyClass: applyClass };
  };

  // --[ applyPatches() ]-------------------------------------------------
  function applyPatches() {
    var elms, selectorText, patches, domSelectorText;

    for (var c=0; c<domPatches.length; c++) {
      selectorText = domPatches[c].selector;
      patches = domPatches[c].patches;

      // Although some selector libraries can find :checked :enabled etc.
      // we need to find all elements that could have that state because
      // it can be changed by the user.
      domSelectorText = selectorText.replace(RE_LIBRARY_INCOMPATIBLE_PSEUDOS, EMPTY_STRING);

      // If the dom selector equates to an empty string or ends with
      // whitespace then we need to append a universal selector (*) to it.
      if (domSelectorText == EMPTY_STRING || domSelectorText.charAt(domSelectorText.length - 1) == SPACE_STRING) {
        domSelectorText += "*";
      }

      // Ensure we catch errors from the selector library
      try {
        elms = selectorMethod( domSelectorText );
      } catch (ex) {
        // #DEBUG_START
        log( "Selector '" + selectorText + "' threw exception '" + ex + "'" );
        // #DEBUG_END
      }


      if (elms) {
        for (var d = 0, dl = elms.length; d < dl; d++) {
          var elm = elms[d];
          var cssClasses = elm.className;
          for (var f = 0, fl = patches.length; f < fl; f++) {
            var patch = patches[f];
            if (!hasPatch(elm, patch)) {
              if (patch.applyClass && (patch.applyClass === true || patch.applyClass(elm) === true)) {
                cssClasses = toggleClass(cssClasses, patch.className, true );
              }
            }
          }
          elm.className = cssClasses;
        }
      }
    }
  };

  // --[ hasPatch() ]-----------------------------------------------------
  // checks for the exsistence of a patch on an element
  function hasPatch( elm, patch ) {
    return new RegExp("(^|\\s)" + patch.className + "(\\s|$)").test(elm.className);
  };
  
  
  // =========================== Utility =================================
  
  function createClassName( className ) {
    return namespace + "-" + ((ieVersion == 6 && patchIE6MultipleClasses) ?
      ie6PatchID++
    :
      className.replace(RE_PATCH_CLASS_NAME_REPLACE, function(a) { return a.charCodeAt(0) }));
  };

  // --[ log() ]----------------------------------------------------------
  // #DEBUG_START
  function log( message ) {
    if (win.console) {
      win.console.log(message);
    }
  };
  // #DEBUG_END

  // --[ trim() ]---------------------------------------------------------
  // removes leading, trailing whitespace from a string
  function trim( text ) {
    return text.replace(RE_TIDY_TRIM_WHITESPACE, PLACEHOLDER_STRING);
  };

  // --[ normalizeWhitespace() ]------------------------------------------
  // removes leading, trailing and consecutive whitespace from a string
  function normalizeWhitespace( text ) {
    return trim(text).replace(RE_TIDY_CONSECUTIVE_WHITESPACE, SPACE_STRING);
  };

  // --[ normalizeSelectorWhitespace() ]----------------------------------
  // tidies whitespace around selector brackets and combinators
  function normalizeSelectorWhitespace( selectorText ) {
    return normalizeWhitespace(selectorText.
      replace(RE_TIDY_TRAILING_WHITESPACE, PLACEHOLDER_STRING).
      replace(RE_TIDY_LEADING_WHITESPACE, PLACEHOLDER_STRING)
    );
  };

  // --[ toggleElementClass() ]-------------------------------------------
  // toggles a single className on an element
  function toggleElementClass( elm, className, on ) {
    var oldClassName = elm.className;
    var newClassName = toggleClass(oldClassName, className, on);
    if (newClassName != oldClassName) {
      elm.className = newClassName;
      elm.parentNode.className += EMPTY_STRING;
    }
  };

  // --[ toggleClass() ]--------------------------------------------------
  // adds / removes a className from a string of classNames. Used to 
  // manage multiple class changes without forcing a DOM redraw
  function toggleClass( classList, className, on ) {
    var re = RegExp("(^|\\s)" + className + "(\\s|$)");
    var classExists = re.test(classList);
    if (on) {
      return classExists ? classList : classList + SPACE_STRING + className;
    } else {
      return classExists ? trim(classList.replace(re, PLACEHOLDER_STRING)) : classList;
    }
  };
  
  // --[ addEvent() ]-----------------------------------------------------
  function addEvent(elm, eventName, eventHandler) {
    elm.attachEvent("on" + eventName, eventHandler);
  };

  // --[ getXHRObject() ]-------------------------------------------------
  function getXHRObject() {
    if (win.XMLHttpRequest) {
      return new XMLHttpRequest;
    }
    try { 
      return new ActiveXObject('Microsoft.XMLHTTP');
    } catch(e) { 
      return null;
    }
  };

  // --[ loadStyleSheet() ]-----------------------------------------------
  function loadStyleSheet( url ) {
    xhr.open("GET", url, false);
    xhr.send();
    return (xhr.status==200) ? xhr.responseText : EMPTY_STRING; 
  };
  
  // --[ resolveUrl() ]---------------------------------------------------
  // Converts a URL fragment to a fully qualified URL using the specified
  // context URL. Returns null if same-origin policy is broken
  function resolveUrl( url, contextUrl, ignoreSameOriginPolicy ) {

    function getProtocolAndHost( url ) {
      return url.substring(0, url.indexOf("/", 8));
    };

    if (!contextUrl) {
      contextUrl = baseUrl;
    }

    // absolute path
    if (/^https?:\/\//i.test(url)) {
      return !ignoreSameOriginPolicy || getProtocolAndHost(contextUrl) == getProtocolAndHost(url) ? url : null;
    }

    // root-relative path
    if (url.charAt(0)=="/") {
      return getProtocolAndHost(contextUrl) + url;
    }

    // relative path
    var contextUrlPath = contextUrl.split(/[?#]/)[0]; // ignore query string in the contextUrl  
    if (url.charAt(0) != "?" && contextUrlPath.charAt(contextUrlPath.length - 1) != "/") {
      contextUrlPath = contextUrlPath.substring(0, contextUrlPath.lastIndexOf("/") + 1);
    }

    return contextUrlPath + url;
  };
  
  // --[ parseStyleSheet() ]----------------------------------------------
  // Downloads the stylesheet specified by the URL, removes it's comments
  // and recursivly replaces @import rules with their contents, ultimately
  // returning the full cssText.
  function parseStyleSheet( url ) {
    if (url) {
      return loadStyleSheet(url).replace(RE_COMMENT, EMPTY_STRING).
      replace(RE_IMPORT, function( match, quoteChar, importUrl, quoteChar2, importUrl2, media ) {
        var cssText = parseStyleSheet(resolveUrl(importUrl || importUrl2, url, true));
        return (media) ? "@media " + media + " {" + cssText + "}" : cssText;
      }).
      replace(RE_ASSET_URL, function( match, quoteChar, assetUrl ) { 
        quoteChar = quoteChar || EMPTY_STRING;
        return " url(" + quoteChar + resolveUrl(assetUrl, url) + quoteChar + ") "; 
      });
    }
    return EMPTY_STRING;
  };

  // --[ getStyleSheets() ]-----------------------------------------------
  function getStyleSheets() {
    var url, stylesheet;
    for (var c = 0; c < doc.styleSheets.length; c++) {
      stylesheet = doc.styleSheets[c];
      if (stylesheet.href != EMPTY_STRING) {
        url = resolveUrl(stylesheet.href);
        if (url) {
          stylesheet.cssText = stylesheet.rawCssText = patchStyleSheet( parseStyleSheet( url ) );
        }
      }
    }
  };

  // --[ init() ]---------------------------------------------------------
  function init() {
    applyPatches();

    // :enabled & :disabled polling script (since we can't hook 
    // onpropertychange event when an element is disabled) 
    if (enabledWatchers.length > 0) {
      setInterval( function() {
        for (var c = 0, cl = enabledWatchers.length; c < cl; c++) {
          var e = enabledWatchers[c];
          if (e.disabled !== e.$disabled) {
            if (e.disabled) {
              e.disabled = false;
              e.$disabled = true;
              e.disabled = true;
            }
            else {
              e.$disabled = e.disabled;
            }
          }
        }
      }, 250)
    }
  };

  // Determine the baseUrl and download the stylesheets
  var baseTags = doc.getElementsByTagName("BASE");
  var baseUrl = (baseTags.length > 0) ? baseTags[0].href : doc.location.href;
  getStyleSheets();

  // Bind selectivizr to the ContentLoaded event. 
  ContentLoaded(win, function() {
    // Determine the "best fit" selector engine
    for (var engine in selectorEngines) {
      var members, member, context = win;
      if (win[engine]) {
        members = selectorEngines[engine].replace("*", engine).split(".");
        while ((member = members.shift()) && (context = context[member])) {}
        if (typeof context == "function") {
          selectorMethod = context;
          init();
          return;
        }
      }
    }
  });
  

  
  /*!
   * ContentLoaded.js by Diego Perini, modified for IE<9 only (to save space)
   *
   * Author: Diego Perini (diego.perini at gmail.com)
   * Summary: cross-browser wrapper for DOMContentLoaded
   * Updated: 20101020
   * License: MIT
   * Version: 1.2
   *
   * URL:
   * http://javascript.nwbox.com/ContentLoaded/
   * http://javascript.nwbox.com/ContentLoaded/MIT-LICENSE
   *
   */

  // @w window reference
  // @f function reference
  function ContentLoaded(win, fn) {

    var done = false, top = true,
    init = function(e) {
      if (e.type == "readystatechange" && doc.readyState != "complete") return;
      (e.type == "load" ? win : doc).detachEvent("on" + e.type, init, false);
      if (!done && (done = true)) fn.call(win, e.type || e);
    },
    poll = function() {
      try { root.doScroll("left"); } catch(e) { setTimeout(poll, 50); return; }
      init('poll');
    };

    if (doc.readyState == "complete") fn.call(win, EMPTY_STRING);
    else {
      if (doc.createEventObject && root.doScroll) {
        try { top = !win.frameElement; } catch(e) { }
        if (top) poll();
      }
      addEvent(doc,"readystatechange", init);
      addEvent(win,"load", init);
    }
  };
})(this);

/*! Respond.js v1.1.0: min/max-width media query polyfill. (c) Scott Jehl. MIT/GPLv2 Lic. j.mp/respondjs  */
(function(e){e.respond={};respond.update=function(){};respond.mediaQueriesSupported=e.matchMedia&&e.matchMedia("only all").matches;if(respond.mediaQueriesSupported){return}var w=e.document,s=w.documentElement,i=[],k=[],q=[],o={},h=30,f=w.getElementsByTagName("head")[0]||s,g=w.getElementsByTagName("base")[0],b=f.getElementsByTagName("link"),d=[],a=function(){var D=b,y=D.length,B=0,A,z,C,x;for(;B<y;B++){A=D[B],z=A.href,C=A.media,x=A.rel&&A.rel.toLowerCase()==="stylesheet";if(!!z&&x&&!o[z]){if(A.styleSheet&&A.styleSheet.rawCssText){m(A.styleSheet.rawCssText,z,C);o[z]=true}else{if((!/^([a-zA-Z:]*\/\/)/.test(z)&&!g)||z.replace(RegExp.$1,"").split("/")[0]===e.location.host){d.push({href:z,media:C})}}}}u()},u=function(){if(d.length){var x=d.shift();n(x.href,function(y){m(y,x.href,x.media);o[x.href]=true;u()})}},m=function(I,x,z){var G=I.match(/@media[^\{]+\{([^\{\}]*\{[^\}\{]*\})+/gi),J=G&&G.length||0,x=x.substring(0,x.lastIndexOf("/")),y=function(K){return K.replace(/(url\()['"]?([^\/\)'"][^:\)'"]+)['"]?(\))/g,"$1"+x+"$2$3")},A=!J&&z,D=0,C,E,F,B,H;if(x.length){x+="/"}if(A){J=1}for(;D<J;D++){C=0;if(A){E=z;k.push(y(I))}else{E=G[D].match(/@media *([^\{]+)\{([\S\s]+?)$/)&&RegExp.$1;k.push(RegExp.$2&&y(RegExp.$2))}B=E.split(",");H=B.length;for(;C<H;C++){F=B[C];i.push({media:F.split("(")[0].match(/(only\s+)?([a-zA-Z]+)\s?/)&&RegExp.$2||"all",rules:k.length-1,hasquery:F.indexOf("(")>-1,minw:F.match(/\(min\-width:[\s]*([\s]*[0-9\.]+)(px|em)[\s]*\)/)&&parseFloat(RegExp.$1)+(RegExp.$2||""),maxw:F.match(/\(max\-width:[\s]*([\s]*[0-9\.]+)(px|em)[\s]*\)/)&&parseFloat(RegExp.$1)+(RegExp.$2||"")})}}j()},l,r,v=function(){var z,A=w.createElement("div"),x=w.body,y=false;A.style.cssText="position:absolute;font-size:1em;width:1em";if(!x){x=y=w.createElement("body")}x.appendChild(A);s.insertBefore(x,s.firstChild);z=A.offsetWidth;if(y){s.removeChild(x)}else{x.removeChild(A)}z=p=parseFloat(z);return z},p,j=function(I){var x="clientWidth",B=s[x],H=w.compatMode==="CSS1Compat"&&B||w.body[x]||B,D={},G=b[b.length-1],z=(new Date()).getTime();if(I&&l&&z-l<h){clearTimeout(r);r=setTimeout(j,h);return}else{l=z}for(var E in i){var K=i[E],C=K.minw,J=K.maxw,A=C===null,L=J===null,y="em";if(!!C){C=parseFloat(C)*(C.indexOf(y)>-1?(p||v()):1)}if(!!J){J=parseFloat(J)*(J.indexOf(y)>-1?(p||v()):1)}if(!K.hasquery||(!A||!L)&&(A||H>=C)&&(L||H<=J)){if(!D[K.media]){D[K.media]=[]}D[K.media].push(k[K.rules])}}for(var E in q){if(q[E]&&q[E].parentNode===f){f.removeChild(q[E])}}for(var E in D){var M=w.createElement("style"),F=D[E].join("\n");M.type="text/css";M.media=E;f.insertBefore(M,G.nextSibling);if(M.styleSheet){M.styleSheet.cssText=F}else{M.appendChild(w.createTextNode(F))}q.push(M)}},n=function(x,z){var y=c();if(!y){return}y.open("GET",x,true);y.onreadystatechange=function(){if(y.readyState!=4||y.status!=200&&y.status!=304){return}z(y.responseText)};if(y.readyState==4){return}y.send(null)},c=(function(){var x=false;try{x=new XMLHttpRequest()}catch(y){x=new ActiveXObject("Microsoft.XMLHTTP")}return function(){return x}})();a();respond.update=a;function t(){j(true)}if(e.addEventListener){e.addEventListener("resize",t,false)}else{if(e.attachEvent){e.attachEvent("onresize",t)}}})(this);