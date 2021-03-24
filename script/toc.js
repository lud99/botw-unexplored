const koroks = require("./koroks.json");
const fs = require("fs");

string = koroks.map(k => {
    return `{ ${k.id}, { "${k.text}", "${k.image}" } }`
}).join(",\n");

fs.writeFileSync("zeldadungeonkoroks.txt", string);