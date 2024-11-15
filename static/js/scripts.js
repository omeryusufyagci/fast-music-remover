
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
        document.getElementById("fileName").textContent = file.name;
        document.getElementById("filePreview").style.display = "flex";
        activeSource = "file";
        setActiveSource("file");
        handleSourceChange(); // needed to ensure active source is reflected properly
    }
}

function removeFile(event) {
    event.stopPropagation();
    uploadedFile = null;
    document.getElementById("fileInput").value = "";
    document.getElementById("filePreview").style.display = "none";
    handleSourceChange();
}

function applyDemoUrl() {
    document.getElementById("url").value = "https://www.youtube.com/watch?v=is6dcedp4w0";
    activeSource = "url";
    handleSourceChange(); // needed to ensure active source is reflected properly
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
            document.getElementById("videoPlayer").src = data.video_url;
            document.getElementById("videoPlayer").style.display = "block";
            status.textContent = "";

            processButton.style.backgroundColor = "#28a745"; // Green
            processButton.textContent = "Completed!";

            setTimeout(() => {
                processButton.style.backgroundColor = "#6c5ce7"; // Purple
                processButton.textContent = "Process Video";
            }, 3000);
        } else {
            status.textContent = data.message || "An error occurred!";
        }
    })
    .catch(() => {
        status.textContent = "An error occurred while connecting to the server.";
    });
}

const uploadArea = document.querySelector(".upload-area");

uploadArea.addEventListener("dragenter", (event) => {
    event.preventDefault();
    uploadArea.classList.add("drag-over");
});

uploadArea.addEventListener("dragover", (event) => {
    event.preventDefault(); // This prevents the video file to be opened on a new tab which is the default behavior
    uploadArea.classList.add("drag-over");
});

uploadArea.addEventListener("dragleave", () => {
    uploadArea.classList.remove("drag-over");
});

uploadArea.addEventListener("drop", (event) => {
    event.preventDefault(); // This prevents the video file to be opened on a new tab which is the default behavior
    uploadArea.classList.remove("drag-over");
    
    if (event.dataTransfer.files.length > 0) {
        handleFileUpload(event.dataTransfer.files[0]);
    }
});