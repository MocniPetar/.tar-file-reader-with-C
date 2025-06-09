import './style.css'
import { addListElement } from './helper.ts'

document.querySelector<HTMLDivElement>('#app')!.innerHTML = `
  <div>
    <header>
      <h1> UNIX Bounty Hunter </h1>
      <h4> Made by Pier Strbad </h4>
    </header>
    <main>
      <h2> .tar File Reader </h2>
      <div class="input-group">
        <input type="file" class="form-control" id="file-upload" aria-describedby="inputGroupFileAddon04" aria-label="Upload">
        <button class="btn btn-outline-secondary" type="button" id="upload-button">Button</button>
      </div>
      <div class="files-box">
        <p> File </p>
        <ul id="list" class="files-list">
        </ul>
      </div>
    </main>
  </div>
`

const input: HTMLInputElement = document.getElementById('file-upload') as HTMLInputElement;
const button = document.getElementById('upload-button');
const list: HTMLUListElement = document.getElementById('list') as HTMLUListElement;

if (button && list && input) {
  button.addEventListener('click', async () => await addListElement(input, list));
}
