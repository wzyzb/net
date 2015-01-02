
var start = 1;

var _html = '<div class="blogpopMain"><div class="l">'
          + '<a href="http://blog.51cto.com/zt/634" target="_blank"><img src="http://s3.51cto.com/wyfs02/M00/27/FA/wKiom1N1pq6jg8LnAAA0dv6PCsk428.jpg" width="105" height="105" /></a>'
          + '<p><a href="http://blog.51cto.com/zt/634" target="_blank">微软公有云学习系列</a></p></div>'
          + '<div class="r"><h4 style="text-align:left;"><a href="http://dog250.blog.51cto.com/2466061/1412926" target="_blank">一个关于Linux Bridge的吐嘈</a></h4>'
          + '<ul>'
          + '<li><a href="http://yaocoder.blog.51cto.com/2668309/1412029" target="_blank">IM系统架构设计之浅见</a></li>'
          + '<li><a href="http://hcymysql.blog.51cto.com/5223301/1411287" target="_blank"style="color:red;">苹果系统MySQL客户端工具一个大坑</a></li>'
          + '<li><a href="http://edu.51cto.com/course/course_id-37.html" target="_blank">Cisco网络工程师和网络安全</a></li>'
          + '<li><a href="http://down.51cto.com/data/1232739" target="_blank"style="color:red;">《51CTO博客月刊》总第5期 发布啦</a></li>'
          + '</ul>'
          + '</div></div>'
          + '';

jQuery('#showMessagerDim').show();

jQuery(function(){
//window.onload=function(){
   if(get_cookie('blog_top')==''&&start==1){
//	 show_pop();
	    jQuery.messager.showblogtop('', _html);
        var date=new Date();
	    var day = 1400601600000;//
	    date.setTime(day);
	    var test = date.getTime();
	    document.cookie="blog_top=yes;domain=.blog.51cto.com;expires="+date.toGMTString()+";path=/;";
    }
	jQuery("#showMessagerDim").click(function(){
		jQuery.messager.showblogtop('', _html);
		//removeIframe();
	});
//}
});


function get_cookie(Name) {
    var search = Name + "=";
    var returnvalue = "";
    if (document.cookie.length > 0) {
        offset = document.cookie.indexOf(search);
        if (offset != -1) {
            offset += search.length;

            end1 = document.cookie.indexOf(";", offset);

            if (end1 == -1)
            end1 = document.cookie.length;
            returnvalue=unescape(document.cookie.substring(offset, end1));
        }
    }
    return returnvalue;
}

