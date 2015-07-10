var cnt = 0;
//time
function showtime()
{
	var Time = new Date();
	var time = Time.toLocaleString();
	document.getElementById("time").innerHTML = time;
	if(Time.getSeconds() % 2 == 0)
		changepicnews();
}
setInterval(function(){showtime()}, 1000);
//end time


//headmenu
function headmenumouseover(ele)
{
	ele.style.backgroundImage = "url(" + "board22.jpg" + ")";
}
function headmenumouseout(ele)
{
	ele.style.backgroundImage = "url(board2.jpg)";
}
//end headmenu


//picnews
var news = new Array("news4.jpg", "news3.jpg", "news2.jpg", "news1.jpg");
var newslinks = new Array("http://www.new1.uestc.edu.cn/specialtopic/7/", "http://www.new1.uestc.edu.cn/specialtopic/5/", "", "");

var picnews = document.getElementById("picnews");

function changepicnews()
{
	picnews.childNodes[1].childNodes[2*cnt+1].style.borderStyle = "solid";
	cnt = (cnt + 3)%4;
	picnews.style.backgroundImage = "url(" + news[cnt] + ")";
	picnews.childNodes[1].childNodes[2*cnt+1].style.borderStyle = "dotted";
}

function picnewsover(ele)
{
	var tmp = ele.getAttribute("name");
	picnews.style.backgroundImage = "url(" + news[tmp] + ")";
}

function picnewsclick()
{
	self.open(newslinks[cnt]);
}
//end picnews