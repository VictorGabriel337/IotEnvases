const signUpButton = document.getElementById("signUp");
const signInButtom = document.getElementById("signIn");
const container = document.getElementById("container");

signUpButton.addEventListener("click", () => {
    container.classList.add("right-panel-active");
});

signInButtom.addEventListener("click", () => {
    container.classList.remove("right-panel-active");
});


document.addEventListener("DOMContentLoaded", function () {
    const botaoEntrar = document.querySelector(".sign-in-container button");

    botaoEntrar.addEventListener("click", function (event) {
        event.preventDefault();

        const usuarioInput = document.querySelector(".sign-in-container input[type='text']").value;
        const senhaInput = document.querySelector(".sign-in-container input[type='password']").value;

        const usuariosValidos = {
            "tamires.menegossi": { senha: "Temp@123", cargo: "Supervisora Industrial", nome: "Tamires Menegossi", img: "../sidebar/img/tamires.jpeg" },
            "ricardo.magni": { senha: "Temp@123", cargo: "Gerente Industrial", nome: "Ricardo Magni", img: "../sidebar/img/ricardo.png" }
        };

        if (usuariosValidos[usuarioInput] && usuariosValidos[usuarioInput].senha === senhaInput) {
            localStorage.setItem("usuarioLogado", JSON.stringify(usuariosValidos[usuarioInput]));
            window.location.href = "../home/inicio.html"; // Redireciona para o sidebar
        } else {
            alert("Usuário ou senha inválidos!");
        }
    });
});
