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

document.addEventListener("DOMContentLoaded", function () {
    const usuarioLogado = JSON.parse(localStorage.getItem("usuarioLogado"));

    if (usuarioLogado) {
        document.querySelector(".user-img img").src = usuarioLogado.img;
        document.querySelector(".user-details .title").textContent = usuarioLogado.cargo;
        document.querySelector(".user-details .name").textContent = usuarioLogado.nome;
    } else {
        // Se não houver usuário logado, redireciona para a página de login
        window.location.href = "../login/index.html";
    }
});
