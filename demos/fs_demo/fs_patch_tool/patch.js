const fs = require('fs');

const file_list = [
    'hello_fs'
];

function zero_buffer(size) {
    let ret = Buffer.alloc(size);
    for (let i = 0; i < size; ++ i) {
        ret[i] = 0;
    }
    return ret;
}

async function main() {
    const vfd_path = `${__dirname}/../a.vfd`;
    const vfd_content = fs.readFileSync(vfd_path);
    console.log('vfd_content length : ', vfd_content.length);
    const vfd_length = vfd_content.length;
    let fs_header = zero_buffer(4);
    const file_cnt = file_list.length;
    fs_header.writeInt32LE(file_cnt);
    for (let i = 0; i < file_cnt; ++ i) {
        const path = `${__dirname}/../root_fs/${file_list[i]}`;
        const content = fs.readFileSync(path);
        const content_length = content.length;
        console.log(`path ${path} : ${content_length}`);
        const file_length_buffer = zero_buffer(4);
        const file_name_buffer = zero_buffer(8);
        file_length_buffer.writeInt32LE(content_length);
        Buffer.from(file_list[i]).copy(file_name_buffer);
        fs_header = Buffer.concat([fs_header, file_length_buffer, file_name_buffer]);
    }
    console.log('fs_header length : ', fs_header.length);

    /*const page_dir_start = 0x8000;
    const header_start_pos = page_dir_start - 120;
    console.log('header_start_pos : ', header_start_pos);
    const buffer_0 = zero_buffer(120-fs_header.length);
    const buffer_before_fs_header = vfd_content.slice(0, header_start_pos);
    const buffer_after_fs_header = vfd_content.slice(page_dir_start);
    const final_buffer = Buffer.concat([buffer_before_fs_header, fs_header, buffer_0, buffer_after_fs_header]);*/

    const start = 0x8ffff;
    //const start = 0xee7ff;
    const p0 = vfd_content.slice(0, start);
    const p1 = zero_buffer(1);
    p1.writeUInt8(0x22);
    const p2 = vfd_content.slice(start+1);
    const final_buffer = Buffer.concat([p0, p1, p2]);
    fs.writeFileSync(vfd_path, final_buffer);
    process.exit();
}
main();