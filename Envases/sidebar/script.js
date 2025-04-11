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
    iframe.style.height = window.innerHeight + "px";  // Ajusta a altura para a altura da janela
    iframe.style.width = "250px";  // Mantém a largura fixa
});