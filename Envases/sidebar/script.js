$(".menu > ul > li").click(function(e){
    $(this).siblings().removeClass("active");

        $(this).toggleClass("active");

        $(this).find("ul").slideToggle();

        $(this).siblings().find("ul").slideUp();

        $(this).siblings().find("ul").find("li").removeClass("active");
        
});

$(".menu-btn").click(function () {
    $(".sidebar").toggleClass("active")
});




window.addEventListener("resize", function() {
    var iframe = document.getElementById('sidebar-iframe');
    if (iframe) {
        iframe.style.height = window.innerHeight + "px";
        iframe.style.width = "250px";
    }
});

const meuElemento = document.getElementById("algumId");
if (meuElemento) {
    meuElemento.style.display = "block";
}