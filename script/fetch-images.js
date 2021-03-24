const fetch = require("node-fetch");
const fs = require("fs");

fs.mkdir("./out", () => {});

const data = require("./zeldadungeon.json");

async function download(url) {
    const response = await fetch(url);
    const buffer = await response.buffer();
    fs.writeFile(`./out/${url.split("/")[url.split("/").length - 1].replace("200px-", "")}`, buffer, () =>
        console.log('finished downloading!'));
}

(async function() {

for (let i = parseInt(process.argv[2]); i < parseInt(process.argv[3]); i++)
{
    download(data[i].image);
}

})();