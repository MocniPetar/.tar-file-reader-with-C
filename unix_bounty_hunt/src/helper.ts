import axios from "axios";

export async function addListElement(input: HTMLInputElement, list: HTMLUListElement) {

  // Send file data and wait for response
  if (input.files) {
    console.log(input.files);

    const fileData = new FormData();
    const url: string = 'http://localhost:8080/readFile';

    fileData.append("file", input.files[0]);
    
    try {
      const response = await axios.post(url, fileData);
      if (response.status === 200) {
        console.log(response.data);
      }
    } catch {
      alert("Unable to send request to " + url);
    }
  }

  const li: HTMLLIElement = document.createElement('li');
  const div: HTMLDivElement = document.createElement('div');
  div.classList.add('file');
  div.innerHTML = `
    <p class="file-name"> File name</p>
    <hr>
    <p class="file-contents"> File contents </p>
  `
  li.appendChild(div);
  list.appendChild(li);
}