window.onload = function() {
    // function verificarTamanhoTela() {
    //     if (window.innerWidth >= 768) {
    //         window.location.href = 'login.html';
    //     } else {
    //         window.location.href = 'mobile.html';
    //     }
    // }

    function verificarTamanhoTela() {
        // Verificar se já estamos na página correta antes de redirecionar
        var currentPage = window.location.pathname.split('/').pop();
        if (window.innerWidth >= 768 && currentPage !== 'login.html') {
            window.location.href = 'login.html';
        } else if (window.innerWidth < 768 && currentPage !== 'mobile.html') {
            window.location.href = 'mobile.html';
        }
    }

    verificarTamanhoTela();

    window.addEventListener('resize', function() {
        verificarTamanhoTela();
    });
};
