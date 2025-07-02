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


 document.addEventListener("DOMContentLoaded", function () {
    const usuario = JSON.parse(localStorage.getItem("usuarioLogado"));

    if (usuario) {
      const imagem = document.getElementById("imagem");
      const nome = document.getElementById("nome");
      const cargo = document.getElementById("cargo");

      imagem.src = usuario.img;
      nome.textContent = usuario.nome;
      cargo.textContent = usuario.cargo;
    } else {
      // Redireciona pro login caso não esteja logado
      window.location.href = "../login/login.html";
    }
  });