let uploadedFile = null;
let activeSource = null;

function handleSourceChange() {
    const url = document.getElementById("url").value.trim();
    const fileToggleContainer = document.querySelector("#fileSection .toggle-container");
    const urlToggleContainer = document.querySelector("#urlSection .toggle-container");

    if (uploadedFile) {
        activeSource = "file";
    } else if (url) {
        activeSource = "url";
    } else {
        activeSource = null;
    }

    if (url && uploadedFile) {
        urlToggleContainer.style.display = "flex";
        fileToggleContainer.style.display = "flex";
    } else {
        urlToggleContainer.style.display = "none";
        fileToggleContainer.style.display = "none";
    }
    setActiveSource(activeSource);
}

function handleFileUpload(file) {
    if (file) {
        uploadedFile = file;

        const uploadArea = document.querySelector(".upload-area");
        
        document.getElementById("fileName").textContent = file.name;
        document.getElementById("uploadText").style.display = "none";
        document.getElementById("fileNameContainer").style.display = "flex";

        uploadArea.classList.add("file-uploaded");

        activeSource = "file";
        setActiveSource("file");
        handleSourceChange();
    }
}
function removeFile(event) {
    event.stopPropagation();
    uploadedFile = null;

    const uploadArea = document.querySelector(".upload-area");
    
    document.getElementById("uploadText").style.display = "block";
    document.getElementById("fileNameContainer").style.display = "none";
    document.getElementById("fileInput").value = "";

    uploadArea.classList.remove("file-uploaded");

    activeSource = document.getElementById("url").value.trim() ? "url" : null;
    setActiveSource(activeSource);

    handleSourceChange();
}

function applyDemoUrl() {
    document.getElementById("url").value = "https://www.youtube.com/watch?v=is6dcedp4w0";
    activeSource = "url";
    handleSourceChange();
}

function toggleActiveSource(source) {
    activeSource = (activeSource === source) ? null : source;
    setActiveSource(activeSource);
}

function setActiveSource(source) {
    document.getElementById("urlSection").classList.toggle("active", source === "url");
    document.getElementById("fileSection").classList.toggle("active", source === "file");
    document.getElementById("urlToggle").classList.toggle("active", source === "url");
    document.getElementById("fileToggle").classList.toggle("active", source === "file");
}

function highlightDemoUrl() {
    document.getElementById("url").classList.add("demo-highlight");
}

function removeHighlightDemoUrl() {
    document.getElementById("url").classList.remove("demo-highlight");
}

function submitForm() {
    const status = document.getElementById("status");
    const processButton = document.querySelector(".button");

    if (!activeSource) {
        status.textContent = "Please select a source (URL or File) to process.";
        return;
    }

    const formData = new FormData();
    if (activeSource === "file") {
        formData.append("file", uploadedFile);
    } else {
        formData.append("url", document.getElementById("url").value);
    }

    status.textContent = "Processing...";
    fetch("/", {
        method: "POST",
        body: formData,
    })
    .then(response => response.json())
    .then(data => {
        if (data.status === "completed") {
            const fileType = data.file_type
            const mediaUrl = data.media_url;

            const activePlayer = fileType === "audio" ? document.getElementById("audioPlayer") : document.getElementById("videoPlayer");
            const inactivePlayer = fileType === "audio" ? document.getElementById("videoPlayer") : document.getElementById("audioPlayer");

            activePlayer.src = mediaUrl;
            activePlayer.style.display = "block";
            inactivePlayer.style.display = "none";

            status.textContent = "";

            processButton.style.backgroundColor = "#28a745"; // Green
            processButton.textContent = "Completed!";

            setTimeout(() => {
                processButton.style.backgroundColor = "#6c5ce7"; // Purple
                processButton.textContent = "Process Media";
            }, 3000);
        } else {
            status.textContent = data.message || "An error occurred!";
        }
    })
    .catch((error) => {
        console.error("Error while connecting to the server:", error);
        status.textContent = "An error occurred while connecting to the server.";
    });
}

const uploadArea = document.getElementById("uploadArea");

uploadArea.addEventListener("dragenter", (event) => {
    event.preventDefault();
    uploadArea.classList.add("drag-over");
});

uploadArea.addEventListener("dragover", (event) => {
    event.preventDefault();
    uploadArea.classList.add("drag-over");
});

uploadArea.addEventListener("dragleave", () => {
    uploadArea.classList.remove("drag-over");
});

uploadArea.addEventListener("drop", (event) => {
    event.preventDefault();
    uploadArea.classList.remove("drag-over");
    
    if (event.dataTransfer.files.length > 0) {
        handleFileUpload(event.dataTransfer.files[0]);
    }
});