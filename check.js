const child = require('child_process');
const fs = require('fs');
const myArgs = process.argv.slice(2);
const checkFile = "cpp_check.txt";

switch (myArgs[0]) {
    case 'check':
        child.exec("pio check --pattern=src/* --json-output --skip-packages", (err, response) => {
            if (err) {
                return console.log(err);
            }
            processChecks(JSON.parse(response));
        });
        break;
    case 'last':
        showLastCheck();
        break;
    default:
        console.log('Sorry, valid arguments are `check` and `last`.');
}

function showLastCheck() {
    let checks = JSON.parse(fs.readFileSync(checkFile));
    processChecks(checks);
}

function processChecks(checks) {
    let orderedChecks = new Array;
    let lowChecks = new Array;
    let midChecks = new Array;
    let severeChecks = new Array;
    checks[0].defects.forEach(defect => {
        let filePathArray = defect.file.split("\\");
        defect.file = filePathArray[filePathArray.length -1];
        let message = defect.message.split(".")[0];
        defect.message = message;

        (defect.severity == "low") ? 
        lowChecks.push(defect) : 
        (defect.severity == "medium") ? 
        midChecks.push(defect) : 
        severeChecks.push(defect);
    });
    orderedChecks = severeChecks.concat(midChecks).concat(lowChecks);
    console.table(orderedChecks, ['severity', 'file', 'line', 'message']);
    fs.writeFile(checkFile, JSON.stringify(checks), "utf8", (err) => {
        if (err) {
            return console.log(err);
        }
    
        console.log("Checks complete, saved to file.");
    });
}
