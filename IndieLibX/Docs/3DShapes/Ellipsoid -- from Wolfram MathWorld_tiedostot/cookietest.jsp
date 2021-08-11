
	            var css = '@-webkit-keyframes rSlide {0% {right: -200%;} 100% {right: 0;}} @keyframes rSlide {0% {right: -200%;} 100% {right: 0;}} #EUCookie p{ text-align: left; color: #999; position: fixed; top: 15px; right: 0px; z-index: 1000; line-height:12px; padding: 15px; background: #fffcf1; width: 176px; border: 1px solid #DC8E00; border-right: none; border-left: 5px solid rgba(20, 92, 137, 0.5); box-shadow: -4px 2px 2px 0 rgba(0,0,0,.25); font-family: Arial, Verdana, Tahoma; font-size: 12px; -webkit-animation: rSlide 2s ease 1 normal; animation: rSlide 2s ease 1 normal;} #EUCookie a:link {color: #B17120 }',
	            head = document.getElementsByTagName('head')[0],
	            style = document.createElement('style');
	
	            style.type = 'text/css';
	            if(style.styleSheet){
	                style.styleSheet.cssText = css;
	            }else{
	                style.appendChild(document.createTextNode(css));
	            }
	            head.appendChild(style);
	
	            var EUC = document.createElement('div');
	            EUC.id = "EUCookie";
	            EUC.innerHTML='<p>Wolfram websites use cookies to improve your experience with our services. By continuing to use this website, you are consenting to this use as described in our  <a href="//www.wolfram.com/legal/privacy/wolfram-research.html" target="_blank">Privacy Policy.</a></p>';
	            if(document.getElementsByTagName("header").length > 0) {
	                document.getElementsByTagName("header")[0].parentNode.insertBefore(EUC,document.getElementsByTagName("header")[0].nextSibling);   
	            } 
	            else {
	                document.body.appendChild(EUC);
	            }
	            EUC.addEventListener('click',function (){ document.body.removeChild(EUC); },false);     
	        