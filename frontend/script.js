function searchProduct() {
    let query = document.getElementById("searchInput").value.trim();
    let sort = document.getElementById("sortSelect").value;

    if (query === "") {
        alert("Please enter a product");
        return;
    }

    // ✅ Encode query (VERY IMPORTANT for "iphone 13")
    let encodedQuery = encodeURIComponent(query);

    // ✅ Update URL (for refresh + back button)
    window.history.pushState({}, "", `?query=${encodedQuery}&sort=${sort}`);

    // ✅ Show loader
    document.getElementById("loader").style.display = "block";

    let url = `/cgi-bin/comparator.exe?query=${encodedQuery}&sort=${sort}`;

    fetch(url)
        .then(response => response.text())
        .then(data => {
            document.getElementById("loader").style.display = "none";
            document.getElementById("results").innerHTML = data;
        })
        .catch(error => {
            document.getElementById("loader").style.display = "none";
            document.getElementById("results").innerHTML = "<p>Error loading data</p>";
            console.log("Error:", error);
        });
}


// ✅ ENTER KEY SUPPORT (important UX)
document.getElementById("searchInput").addEventListener("keypress", function(e) {
    if (e.key === "Enter") {
        searchProduct();
    }
});


// ✅ PAGE LOAD (refresh support)
window.onload = function () {
    let params = new URLSearchParams(window.location.search);

    let query = params.get("query");
    let sort = params.get("sort") || "price";

    if (query) {
        // decode for input field
        document.getElementById("searchInput").value = decodeURIComponent(query);
    }

    document.getElementById("sortSelect").value = sort;

    if (query) {
        document.getElementById("loader").style.display = "block";

        let url = `/cgi-bin/comparator.exe?query=${query}&sort=${sort}`;

        fetch(url)
            .then(response => response.text())
            .then(data => {
                document.getElementById("loader").style.display = "none";
                document.getElementById("results").innerHTML = data;
            });
    } else {
        document.getElementById("results").innerHTML = "";
    }
};