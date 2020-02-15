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
    const vfd_length = vfd_content.length;
    let fs_header = zero_buffer(4);
    fs_header.writeInt32LE(vfd_length);
    const file_cnt = file_list.length;
    const file_cnt_buffer = zero_buffer(4);
    fs_header = Buffer.concat([fs_header, file_cnt_buffer]);
    for (let i = 0; i < file_cnt; ++ i) {
        const path = `${__dirname}/../root_fs/${file_list[i]}`;
        const content = fs.readFileSync(path);
        const content_length = content.length;
        const file_length_buffer = zero_buffer(4);
        const file_name_buffer = zero_buffer(8);
        file_length_buffer.writeInt32LE(content_length);
        for (let j = 0; j < 8 && j < content_length; ++ j) {
            file_name_buffer[j] = file_list[i][j];
        }
        fs_header = Buffer.concat([fs_header, file_length_buffer, file_name_buffer]);
    }
    console.log('fs_header length : ', fs_header.length);

    const page_dir_start = 0x8000;
    const header_start_pos = page_dir_start - 124;
    const buffer_0 = Buffer.alloc(124-fs_header.length);
    const buffer_before_fs_header = vfd_content.slice(0, header_start_pos);
    const buffer_after_fs_header = vfd_content.slice(page_dir_start);
    const final_buffer = Buffer.concat([buffer_before_fs_header, fs_header, buffer_0, buffer_after_fs_header]);
    fs.writeFileSync(vfd_path, final_buffer);
    process.exit();
}
main();