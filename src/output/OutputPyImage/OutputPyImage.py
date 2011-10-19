

<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta content="text/html; charset=utf-8" http-equiv="Content-Type" />
<meta content="Loggerhead/1.18 Python/2.6.5-r6152 Bazaar/2.5.0dev2-r6152 Paste/1.7.2 PasteDeploy/1.3.3 SimpleTAL/4.1 Pygments/1.4 simplejson/2.1.3" name="generator" />
<title>~michaelj-h/maus/devel : revision 669</title>
<link href="/static/css/global.css" rel="stylesheet" />
<link href="/static/images/favicon.png" rel="shortcut icon" />

<script type="text/javascript">
var global_path = '/~michaelj-h/maus/devel/';
var collapsed_icon_path = '/static/images/treeCollapsed.png';
var expanded_icon_path = '/static/images/treeExpanded.png';
</script>
<script src="/static/javascript/yui/build/yui/yui-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/oop/oop-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/event/event-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/attribute/attribute-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/base/base-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/dom/dom-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/node/node-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/anim/anim-min.js" type="text/javascript"></script>
<script src="/static/javascript/yui/build/io/io-base-min.js" type="text/javascript"></script>
<script src="/static/javascript/custom.js" type="text/javascript"></script>

<link href="/static/css/diff.css" media="all" type="text/css" rel="stylesheet" />
<script src="/static/javascript/diff.js" type="text/javascript"></script>
<script type="text/javascript">
var link_data = {};
var specific_path = "src/output/OutputPyImage/OutputPyImage.py";
var path_to_id = {};
</script>

</head>
<body>



<div class="black-link">
<a href="https://code.launchpad.net/~michaelj-h/maus/devel">
← Back to branch summary
</a>
</div>


<h1 class="branch-name">
~michaelj-h/maus/devel
</h1>

<ul id="menuTabs">

<li><a href="/~michaelj-h/maus/devel/changes" title="Changes" id="on">Changes</a></li>
<li><a href="/~michaelj-h/maus/devel/files" title="Files">Files</a></li>


</ul>

<div id="loggerheadCont">
<div id="search_terms"></div>

<div id="breadcrumbs">

<a href="https://code.launchpad.net/~michaelj-h/maus/devel">~michaelj-h/maus/devel</a>


<span class="breadcrumb">» Revision

<a href="/~michaelj-h/maus/devel/revision/669" title="View changes to all files">669</a>

</span>
<span class="breadcrumb">
: <a href="/~michaelj-h/maus/devel/view/669/src/output/OutputPyImage/OutputPyImage.py" title="Annotate src/output/OutputPyImage/OutputPyImage.py">src/output/OutputPyImage/OutputPyImage.py</a>
</span>
</div>


<p>
<a href="/~michaelj-h/maus/devel/revision/669">
« back to all changes in this revision
</a>
</p>
<p>
Viewing changes to <a href="/~michaelj-h/maus/devel/view/669/src/output/OutputPyImage/OutputPyImage.py" title="Annotate src/output/OutputPyImage/OutputPyImage.py">src/output/OutputPyImage/OutputPyImage.py</a>
</p>
<ul id="submenuTabs">
<li id="first"><a href="/~michaelj-h/maus/devel/files/669" title="browse files at revision 669">browse files at revision 669</a></li>
<li>
<a href="/~michaelj-h/maus/devel/revision/669?remember=669" title="compare with another revision">Compare with another revision</a></li>

<li>
<a href="/~michaelj-h/maus/devel/diff/669">download diff</a>

</li>
<li id="last"><a href="/~michaelj-h/maus/devel/changes/669" title="view history from revision 669">view history from revision 669</a></li>
</ul>

<div class="infoContainer">
<div id="infTxt">
<ul>
<li class="committer">
<strong>Committer:</strong>
<span>Mike Jackson</span>
</li>

<li class="timer">
<strong>Date:</strong>
<span>2011-10-19 12:31:17</span>
</li>



<li class="revid">
<strong>Revision ID:</strong>
<span>michaelj@epcc.ed.ac.uk-20111019123117-a31gue7hfcq3enuo</span>
</li>
</ul>


<div class="clear"></div>

<div class="information">Fixed bug 750. Now just uses a string, not a UUID. Caller can override via configuration parameter if they don&#39;t want the string.</div>
</div>
<ul id="list-files">




<ul>

<li class="desc">files modified:</li>

<li class="files">
<b><a href="#src/output/OutputPyImage/OutputPyImage.py">src/output/OutputPyImage/OutputPyImage.py</a></b>
</li>
</ul>

</ul>
<div class="clear"></div>
</div>


<p class="expand show_if_js"><a href="#" id="toggle_unified_sbs">Show diffs side-by-side</a></p>
<p class="codin"><img src="/static/images/newCode.gif" alt="added" /> added</p>
<p class="codin"><img src="/static/images/deleteCode.gif" alt="removed" /> removed</p>
<div class="clear"></div>

<div>

<div class="diff">
<div class="diffBox">
<a href="/~michaelj-h/maus/devel/revision/669/src/output/OutputPyImage/OutputPyImage.py" id="src/output/OutputPyImage/OutputPyImage.py" title="View changes to src/output/OutputPyImage/OutputPyImage.py only" class="the-link">
<img src="/static/images/treeExpanded.png" class="expand_diff" />
src/output/OutputPyImage/OutputPyImage.py
</a>
</div>
<div style="overflow: hidden">
<div class="container">
<div style="display:none" class="loading">
<img src="/static/images/spinner.gif" />
</div>
<div class="diffinfo">
<div class="pseudotable unified">

<div class="pseudorow context-row">
<div class="lineNumber first">20</div>
<div class="lineNumber second">20</div>
<div class="code context">import&nbsp;io</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">21</div>
<div class="lineNumber second">21</div>
<div class="code context">import&nbsp;json</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">22</div>
<div class="lineNumber second">22</div>
<div class="code context">import&nbsp;os</div>
<div class="clear"></div>
</div><div class="pseudorow delete-row">
<div class="lineNumber first">23</div>
<div class="lineNumber second">&nbsp;</div>
<div class="code delete">import&nbsp;uuid</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">24</div>
<div class="lineNumber second">23</div>
<div class="code context">&nbsp;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">25</div>
<div class="lineNumber second">24</div>
<div class="code context">class&nbsp;OutputPyImage:</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">26</div>
<div class="lineNumber second">25</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&quot;&quot;&quot;</div>
<div class="clear"></div>
</div>
</div><div class="pseudotable unified">

<div class="pseudorow context-row">
<div class="lineNumber separate"></div>
<div class="lineNumber second separate"></div>
<div class="code separate"></div>
<div class="clear"></div>
</div>

<div class="pseudorow context-row">
<div class="lineNumber first">46</div>
<div class="lineNumber second">45</div>
<div class="code context">&nbsp;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">47</div>
<div class="lineNumber second">46</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;The&nbsp;caller&nbsp;can&nbsp;configure&nbsp;the&nbsp;worker&nbsp;and&nbsp;specify:</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">48</div>
<div class="lineNumber second">47</div>
<div class="code context">&nbsp;</div>
<div class="clear"></div>
</div><div class="pseudorow delete-row">
<div class="lineNumber first">49</div>
<div class="lineNumber second">&nbsp;</div>
<div class="code delete">&nbsp;&nbsp;&nbsp;&nbsp;-File&nbsp;prefix&nbsp;(&quot;image_file_prefix&quot;).&nbsp;Default:&nbsp;auto-generated</div>
<div class="clear"></div>
</div><div class="pseudorow delete-row">
<div class="lineNumber first">50</div>
<div class="lineNumber second">&nbsp;</div>
<div class="code delete">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;UUID.&nbsp;</div>
<div class="clear"></div>
</div><div class="pseudorow insert-row">
<div class="lineNumber first">&nbsp;</div>
<div class="lineNumber second">48</div>
<div class="code insert">&nbsp;&nbsp;&nbsp;&nbsp;-File&nbsp;prefix&nbsp;(&quot;image_file_prefix&quot;).&nbsp;Default:&nbsp;&quot;image&quot;.</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">51</div>
<div class="lineNumber second">49</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;-Directory&nbsp;for&nbsp;files&nbsp;(&quot;image_directory&quot;).&nbsp;Default:&nbsp;current</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">52</div>
<div class="lineNumber second">50</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;working&nbsp;directory.&nbsp;If&nbsp;the&nbsp;given&nbsp;directory&nbsp;does&nbsp;not&nbsp;exist&nbsp;it</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">53</div>
<div class="lineNumber second">51</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;will&nbsp;be&nbsp;created.&nbsp;This&nbsp;can&nbsp;be&nbsp;absolute&nbsp;or&nbsp;relative.</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">54</div>
<div class="lineNumber second">52</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&quot;&quot;&quot;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">55</div>
<div class="lineNumber second">53</div>
<div class="code context">&nbsp;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">56</div>
<div class="lineNumber second">54</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;def&nbsp;__init__(self):</div>
<div class="clear"></div>
</div><div class="pseudorow delete-row">
<div class="lineNumber first">57</div>
<div class="lineNumber second">&nbsp;</div>
<div class="code delete">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;self.file_prefix&nbsp;=&nbsp;&quot;&quot;</div>
<div class="clear"></div>
</div><div class="pseudorow insert-row">
<div class="lineNumber first">&nbsp;</div>
<div class="lineNumber second">55</div>
<div class="code insert">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;self.file_prefix&nbsp;=&nbsp;&quot;image&quot;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">58</div>
<div class="lineNumber second">56</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;self.directory&nbsp;=&nbsp;os.getcwd()</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">59</div>
<div class="lineNumber second">57</div>
<div class="code context">&nbsp;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">60</div>
<div class="lineNumber second">58</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;def&nbsp;birth(self,&nbsp;config_json):</div>
<div class="clear"></div>
</div>
</div><div class="pseudotable unified">

<div class="pseudorow context-row">
<div class="lineNumber separate"></div>
<div class="lineNumber second separate"></div>
<div class="code separate"></div>
<div class="clear"></div>
</div>

<div class="pseudorow context-row">
<div class="lineNumber first">71</div>
<div class="lineNumber second">69</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;key&nbsp;=&nbsp;&quot;image_file_prefix&quot;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">72</div>
<div class="lineNumber second">70</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;if&nbsp;key&nbsp;in&nbsp;config_doc:</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">73</div>
<div class="lineNumber second">71</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;self.file_prefix&nbsp;=&nbsp;config_doc[key]</div>
<div class="clear"></div>
</div><div class="pseudorow delete-row">
<div class="lineNumber first">74</div>
<div class="lineNumber second">&nbsp;</div>
<div class="code delete">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;else:</div>
<div class="clear"></div>
</div><div class="pseudorow delete-row">
<div class="lineNumber first">75</div>
<div class="lineNumber second">&nbsp;</div>
<div class="code delete">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;self.file_prefix&nbsp;=&nbsp;uuid.uuid4()</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">76</div>
<div class="lineNumber second">72</div>
<div class="code context">&nbsp;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">77</div>
<div class="lineNumber second">73</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;key&nbsp;=&nbsp;&quot;image_directory&quot;</div>
<div class="clear"></div>
</div><div class="pseudorow context-row">
<div class="lineNumber first">78</div>
<div class="lineNumber second">74</div>
<div class="code context">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;if&nbsp;key&nbsp;in&nbsp;config_doc:</div>
<div class="clear"></div>
</div>
</div>
</div>
</div>
</div>
</div>


<ul id="pages">
<li class="previous">
<a href="/~michaelj-h/maus/devel/revision/670?start_revid=670"> Newer</a>
</li>

<li class="next">
<a href="/~michaelj-h/maus/devel/revision/668?start_revid=670">Older </a>
</li>
</ul>
</div>
<p id="footer" class="fl">Loggerhead 1.18 is a web-based interface for <a href="http://bazaar-vcs.org/">Bazaar</a> branches</p>
</div>
</body>
</html>
